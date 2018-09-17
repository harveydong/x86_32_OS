#include <linux/init.h>
#include <linux/linkage.h>
#include <asm/mpspec.h>
#include <asm/fixmap.h>
#include <asm/apic.h>
#include <asm/io_apic.h>

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

asmlinkage void smp_error_interrupt(void)
{

}
