#include <linux/init.h>
#include <linux/linkage.h>
#include <asm/mpspec.h>
#include <asm/fixmap.h>
#include <asm/apic.h>
#include <asm/io_apic.h>
#include <asm/ptrace.h>
#include <asm/apic.h>
#include <asm/hardirq.h>
#include <asm/apicdef.h>
#include <linux/sched.h>
#include <asm/hw_irq.h>
#include <linux/printk.h>


int prof_multiplier[NR_CPUS] = {1,};
int prof_old_multiplier[NR_CPUS] = {1,};
int prof_counter[NR_CPUS] = {1,};

unsigned int apic_timer_irqs[NR_CPUS];

static unsigned int calibration_result;

#define APIC_DIVISOR 16

unsigned volatile long global_irq_lock;

static void __setup_APIC_LVTT(unsigned int clocks)
{
	unsigned int lvttl_value,tmp_value;
	
	lvttl_value = SET_APIC_TIMER_BASE(APIC_TIMER_BASE_DIV) | APIC_LVT_TIMER_PERIODIC|LOCAL_TIMER_VECTOR;
	
	apic_write_around(APIC_LVTT,lvttl_value);
	
	tmp_value = apic_read(APIC_TDCR);
	apic_write_around(APIC_TDCR,(tmp_value & ~(APIC_TDR_DIV_1 | APIC_TDR_DIV_TMBASE))|APIC_TDR_DIV_16);

	apic_write_around(APIC_TMICT,clocks/APIC_DIVISOR);
}

static inline void smp_local_timer_interrupt(struct pt_regs *regs)
{
	int user = user_mode(regs);
	int cpu = smp_processor_id();

	if(!user)
		x86_do_profile(regs->eip);

	if(--prof_counter[cpu] <= 0){
		
		prof_counter[cpu] = prof_multiplier[cpu];
		if(prof_counter[cpu] != prof_old_multiplier[cpu]){
			__setup_APIC_LVTT(calibration_result/prof_counter[cpu]);
			prof_old_multiplier[cpu] = prof_counter[cpu];
		}
		update_process_times(user);
	}

	
}
void smp_apic_timer_interrupt(struct pt_regs *regs)
{
	int cpu = smp_processor_id();

	apic_timer_irqs[cpu]++;
	ack_APIC_irq();

	irq_enter(cpu,0);
	smp_local_timer_interrupt(regs);
	irq_exit(cpu,0);
}

asmlinkage void smp_spurious_interrupt(void)
{
	unsigned long v;
	
	v = apic_read(APIC_ISR + ((SPURIOUS_APIC_VECTOR&~0x1f) >> 1));
	if(v & (1 << (SPURIOUS_APIC_VECTOR&0x1f)))
		ack_APIC_irq();

	printk("spurious APIC interupt on CPU#%d,should never happend\n",smp_processor_id());
}


asmlinkage void smp_error_interrupt(void)
{
	unsigned long v,vl;
	
	v = apic_read(APIC_ESR);
	apic_write(APIC_ESR,0);
	vl = apic_read(APIC_ESR);

	ack_APIC_irq();
	irq_err_count++;
	printk("APIC error on CPU#%d:%021x(%021x)\n",smp_processor_id(),v,vl);
}

void __init init_apic_mapping(void)
{

	unsigned long apic_phys;

	if(smp_found_config)
		apic_phys = mp_lapic_addr;
	else{
		printk("Use default LOCAL APIC addr\n");
	}	

	set_fixmap_nocache(FIX_APIC_BASE,apic_phys);
	printk("mapped APIC to %x (%x)\n",APIC_BASE,apic_phys);

	if(boot_cpu_id == -1U)
		boot_cpu_id = GET_APIC_ID(apic_read(APIC_ID));

	
	printk("boot cpu id:0x%x\n",boot_cpu_id);

	{
		unsigned long ioapic_phys,idx = FIX_IO_APIC_BASE_0;

		int i;
		
		for(i = 0; i < nr_ioapics;i++){

			if(smp_found_config){
				ioapic_phys = mp_ioapics[i].mpc_apicaddr;
			}else{

				printk("No found smp config for ioapic\n");
				while(1);
			}

			set_fixmap_nocache(idx,ioapic_phys);
			printk("mapped IOAPIC to %x (%x)\n",__fix_to_virt(idx),ioapic_phys);
			idx++;
		}
	}
	
}

