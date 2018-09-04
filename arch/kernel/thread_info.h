#ifndef __THREAD_INFO_H_
#define __THREAD_INFO_H_
#include "types.h"
#include "uaccess.h"
#include "const.h"

//#include "sched.h"

#define PREEMPT_ACTIVE	0x10000000
#define INIT_PREEMPT_COUNT (1+PREEMPT_ACTIVE)

struct task_struct;
struct thread_info {
	struct task_struct *task;
//	struct exec_domain *exec_domain;
	unsigned long flags;
	__u32 status;
	
	__u32 cpu;
	int preempt_count;
	mm_segment_t addr_limit;
//	struct restart_block restart_block;
	void __user *sysenter_return;
	int uaccess_err;

};


#define INIT_THREAD_INFO(tsk) \
{				\
	.task = &tsk,\	
	.flags = 0,\
	.cpu = 0,\
	.preempt_count  = INIT_PREEMPT_COUNT,\
	.addr_limit = KERNEL_DS,\ 
}


#define init_thread_info (init_thread_union.thread_info)

#endif
