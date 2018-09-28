#ifndef __INTERRUPT_H_
#define __INTERRUPT_H_
#include <linux/linkage.h>
#include <asm/ptrace.h>

struct irqaction {
	void (*handler)(int, void *, struct pt_regs *);
	unsigned long flags;
	unsigned long mask;
	const char *name;
	void *dev_id;
	struct irqaction *next;
};

extern spinlock_t global_bh_lock;

asmlinkage void do_softirq(void);


enum{
	TIMER_BH = 0,
	TQUEUE_BH

};

extern void init_bh(int nr,void (*routine)(void));
#endif
