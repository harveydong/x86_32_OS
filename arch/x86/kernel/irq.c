#include <linux/spinlock.h>
#include <linux/irq.h>
#include <asm/irq.h>
#include <asm/apic.h>
#include <asm/signal.h>
#include <asm/system.h>
#include <linux/irq_cpustat.h>
#include <asm/bitops.h>
#include <asm/hardirq.h>
#include <asm/smp.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/linkage.h>
#include <linux/printk.h>
#include <asm/errno.h>
#include <linux/kernel_stat.h>

volatile unsigned long irq_err_count;

static unsigned int startup_none(unsigned int irq)
{
	return 0;
}

static void enable_none(unsigned int irq)
{

}
static void disable_none(unsigned int irq)
{

}

static void ack_none(unsigned int irq)
{
	printk("unexped IRQ trap at vector %02x\n",irq);
	ack_APIC_irq();
}

#define shutdown_none disable_none
#define end_none enable_none

struct hw_interrupt_type no_irq_type = {
	.name = "none",
	.startup = startup_none,
	.shutdown = shutdown_none,
	.enable = enable_none,
	.disable = disable_none,
	.ack = ack_none,
	.end = end_none 

};

irq_desc_t irq_desc[NR_IRQS] = {

	[0 ... NR_IRQS-1] = {
		.status = 0,
		.handler = &no_irq_type,
		.action = NULL,
		.depth = 0,
		.lock = SPIN_LOCK_UNLOCKED
	}
};

int setup_irq(unsigned int irq,struct irqaction *new)
{
	int shared = 0;
	unsigned long flags;
	struct irqaction *old,**p;
	irq_desc_t *desc = irq_desc + irq;

	
	if(new->flags & SA_SAMPLE_RANDOM){

	}

	spin_lock_irqsave(&desc->lock,flags);

	p = &desc->action;
	if((old = *p) != NULL){
		if(!(old->flags & new->flags & SA_SHIRQ)){
			spin_unlock_irqrestore(&desc->lock,flags);
			return -EBUSY;
		}
		
		do{
			p = &old->next;
			old = *p;
		}while(old);
		
		shared = 1;
	}


	*p = new;

	printk("irq is %d,and shared:%d\n",irq,shared);
	if(!shared){
		desc->depth = 0;
		desc->status &= ~(IRQ_DISABLED|IRQ_AUTODETECT|IRQ_WAITING);
		desc->handler->startup(irq);
	}

	spin_unlock_irqrestore(&desc->lock,flags);
	return 0;
}


unsigned char global_irq_holder = NO_PROC_ID;
unsigned volatile long global_irq_lock;

#define MAXCOUNT 10000000
#define EFLAGS_IF_SHIFT 9

static inline void wait_on_irq(int cpu)
{
	int count = MAXCOUNT;

	for(;;){
		if(!irqs_running())
			if(local_bh_count(cpu) || !spin_is_locked(&global_bh_lock))
				break;

		clear_bit(0,&global_irq_lock);

		for(;;){
			if(!--count){
				printk("wait on irq\n");
				count = ~0;
			}
			__sti();
			__cli();
			if(irqs_running())
				continue;
			if(global_irq_lock)
				continue;	
			if(!local_bh_count(cpu) && spin_is_locked(&global_bh_lock))
				continue;
			if(!test_and_set_bit(0,&global_irq_lock))
				break;
		}	
	}
}

static inline void get_irqlock(int cpu)
{
	if(test_and_set_bit(0,&global_irq_lock)){
		if((unsigned char)cpu == global_irq_holder)
			return;

		do{
			do{

			}while(test_bit(0,&global_irq_lock));
		}while(test_and_set_bit(0,&global_irq_lock));
	}

	wait_on_irq(cpu);

	global_irq_holder = cpu;
}
void __global_cli(void)
{
	unsigned int flags;

	__save_flags(flags);
	if(flags & (1<<EFLAGS_IF_SHIFT)){
		int cpu = smp_processor_id();
		__cli();

		if(!local_irq_count(cpu))
			get_irqlock(cpu);
	}
}


void __global_sti(void)
{
	int cpu = smp_processor_id();
	

	if(!local_irq_count(cpu))
		release_irqlock(cpu);

	__sti();
}

static int handle_IRQ_event(unsigned int irq,struct pt_regs *regs,struct irqaction *action)
{
	int status;
	int cpu = smp_processor_id();

	irq_enter(cpu,irq);
	status = 1;

	if(!(action->flags & SA_INTERRUPT))
		__sti();

	do{
		status |= action->flags;
		action->handler(irq,action->dev_id,regs);
		action = action->next;
	}while(action);

	__cli();

	irq_exit(cpu,irq);

	return status;
}

asmlinkage unsigned int do_IRQ(struct pt_regs regs)
{

	int irq = regs.orig_eax & 0xff;

	int cpu = smp_processor_id();
	irq_desc_t *desc = irq_desc + irq;

	struct irqaction *action;

	unsigned int status;
	kstat.irqs[cpu][irq]++;

	spin_lock(&desc->lock);
	desc->handler->ack(irq);

	status = desc->status & ~(IRQ_REPLAY|IRQ_WAITING);
	status |= IRQ_PENDING;
	
	action = NULL;
	if(!(status & (IRQ_DISABLED | IRQ_INPROGRESS))){
		action = desc->action;
		status &= ~IRQ_PENDING;
		status |= IRQ_INPROGRESS;
	}

	desc->status = status;
	
	if(!action)
		goto out;
		
	for(;;){
		spin_unlock(&desc->lock);
		handle_IRQ_event(irq,&regs,action);
		spin_lock(&desc->lock);
		
		if(!(desc->status & IRQ_PENDING))
			break;

		desc->status &= ~IRQ_PENDING;
	}

	desc->status &= ~IRQ_INPROGRESS;


out:
	desc->handler->end(irq);
	spin_unlock(&desc->lock);
		
	printk("into do IRQ so here,and irq is 0x%x\n\n",irq);

	return 0;

}
