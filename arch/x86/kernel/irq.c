#include <linux/spinlock.h>
#include <linux/irq.h>
#include <asm/irq.h>
#include <asm/apic.h>
#include <asm/signal.h>

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
}
