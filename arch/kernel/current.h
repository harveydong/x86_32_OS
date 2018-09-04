#ifndef __CURRENT_H_
#define __CURRENT_H_
#include "percpu_defs.h"
//#include "percpu_fun.h"
#include "init.h"
#include "sched.h"
#include "printk.h"
struct task_struct;

DECLARE_PER_CPU(struct task_struct *,current_task);
//extern __attribute__((section(".data..percpu"))) __typeof__(struct task_struct *) current_task;

static inline struct task_struct *get_current(void)
{
	struct task_struct *pfo_;
	asm volatile("movq %%fs:%1,%0":"=r"(pfo_):"m"((current_task)));
	return pfo_;// percpu_read_stable(current_task);
}
#define current get_current()
#endif
