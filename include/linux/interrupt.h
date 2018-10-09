#ifndef __INTERRUPT_H_
#define __INTERRUPT_H_
#include <linux/linkage.h>
#include <asm/ptrace.h>
#include <linux/sched.h>
#include <linux/irq_cpustat.h>
#include <asm/bitops.h>

struct tasklet_struct
{
	struct tasklet_struct *next;
	unsigned long state;
	atomic_t count;
	void (*func)(unsigned long);
	unsigned long data;
};


struct tasklet_head
{
	struct tasklet_struct *list;
};

struct softirq_action
{
	void	(*action)(struct softirq_action *);
	void	*data;
};



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
enum{
	HI_SOFTIRQ=0,
	NET_TX_SOFTIRQ,
	NET_RX_SOFTIRQ,
	TASKLET_SOFTIRQ
};



enum
{
	TASKLET_STATE_SCHED,	/* Tasklet is scheduled for execution */
	TASKLET_STATE_RUN	/* Tasklet is running (SMP only) */
};



#define tasklet_trylock(t) (!test_and_set_bit(TASKLET_STATE_RUN, &(t)->state))
#define tasklet_unlock_wait(t) while (test_bit(TASKLET_STATE_RUN, &(t)->state)) { /* NOTHING */}
#define tasklet_unlock(t) clear_bit(TASKLET_STATE_RUN, &(t)->state)


static inline void __cpu_raise_softirq(int cpu, int nr)
{
	softirq_active(cpu) |= (1<<nr);
}


extern struct tasklet_head tasklet_vec[NR_CPUS];
extern struct tasklet_head tasklet_hi_vec[NR_CPUS];


static inline void tasklet_hi_schedule(struct tasklet_struct *t)
{
	if (!test_and_set_bit(TASKLET_STATE_SCHED, &t->state)) {
		int cpu = smp_processor_id();
		unsigned long flags;

		local_irq_save(flags);
		t->next = tasklet_hi_vec[cpu].list;
		tasklet_hi_vec[cpu].list = t;
		__cpu_raise_softirq(cpu, HI_SOFTIRQ);
		local_irq_restore(flags);
	}
}



extern struct tasklet_struct bh_task_vec[];
static inline void mark_bh(int nr)
{
	tasklet_hi_schedule(bh_task_vec+nr);
}


extern void init_bh(int nr,void (*routine)(void));
#endif
