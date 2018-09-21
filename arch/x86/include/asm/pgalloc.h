#ifndef __PGALLOC_H_
#define __PGALLOC_H_

#include <linux/sched.h>
#include <linux/threads.h>

#define TLBSTATE_OK 1
#define TLBSTATE_LAZY 2

struct tlb_state{
	struct mm_struct *active_mm;
	int state;
};

extern struct tlb_state cpu_tlbstate[NR_CPUS];
#endif
