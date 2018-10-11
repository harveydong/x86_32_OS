#ifndef __IRQ_H_
#define __IRQ_H_
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <asm/irq.h>

#define IRQ_INPROGRESS 1
#define IRQ_DISABLED 2
#define IRQ_PENDING 4
#define IRQ_AUTODETECT 16
#define IRQ_WAITING 32

#define IRQ_REPLAY 8

struct hw_interrupt_type{

	const char *name;
	unsigned int (*startup)(unsigned int irq);
	void (*shutdown)(unsigned int irq);
	void (*enable)(unsigned int irq);
	void (*disable)(unsigned int irq);
	void (*ack)(unsigned int irq);
	void (*end)(unsigned int irq);
	void (*set_affinity)(unsigned int irq,unsigned int mask);
};

typedef struct hw_interrupt_type hw_irq_controller;

typedef struct{
	unsigned int status;
	unsigned int depth;
	hw_irq_controller *handler;
	struct irqaction *action;
	spinlock_t lock;
	
}irq_desc_t;

extern irq_desc_t irq_desc[NR_IRQS];

extern hw_irq_controller no_irq_type;
#endif
