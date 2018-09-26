#include <linux/spinlock.h>
#include <linux/irq.h>
#include <asm/irq.h>

struct hw_interrupt_type no_irq_type = {
	.name = "none",

};

irq_desc_t irq_desc[NR_IRQS] __cacheline_aligned = {

	[0 ... NR_IRQS-1] = {
		. = 0,
		. = &no_irq_type,
		. = NULL,
		. = 0,
		. = SPIN_LOCK_UNLOCKED
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
