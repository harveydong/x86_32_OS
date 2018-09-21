#ifndef __MMU_CONTEXT_H_
#define __MMU_CONTEXT_H_

#include <asm/pgalloc.h>
#include <linux/sched.h>

static inline void enter_lazy_tlb(struct mm_struct *mm,struct task_struct *tsk,unsigned cpu)
{
	if(cpu_tlbstate[cpu].state == TLBSTATE_OK)
		cpu_tlbstate[cpu].state = TLBSTATE_LAZY;

}
#endif
