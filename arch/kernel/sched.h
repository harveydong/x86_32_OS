#ifndef __SCHED_H_
#define __SCHED_H_
#include "page_64_types.h"
#include "thread_info.h"
#include "list.h"
//#include "mm_types.h"

#define MAX_USER_RT_PRIO 100
#define MAX_RT_PRIO MAX_USER_RT_PRIO
#define MAX_PRIO (MAX_RT_PRIO + 40)

#define PF_KTHREAD 0x00200000

#define SCHED_NORMAL 0

union thread_union {
	struct thread_info thread_info;
	unsigned long stack[THREAD_SIZE/sizeof(long)];
};

struct task_struct{
	volatile long state;
	void *stack;
	unsigned int flags;
	int prio,static_prio;
	unsigned int policy;
	struct mm_struct *mm,*active_mm;
	struct list_head tasks;
	unsigned long stack_canary;
	
};
extern union thread_union init_thread_union;
extern struct task_struct init_task;
extern struct mm_struct init_mm;

#endif

