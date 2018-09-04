#ifndef __INIT_TASK_H_
#define __INIT_TASK_H_
#include "sched.h"
#include "list.h"
#include "thread_info.h"

#define INIT_TASK(tsk)	\
{			\
	.state = 0,	\
	.stack = &init_thread_info,	\	
	.flags = PF_KTHREAD,		\
	.prio = MAX_PRIO - 20,		\
	.static_prio = MAX_PRIO - 20,	\
	.policy = SCHED_NORMAL,		\
	.mm = NULL,			\
	.active_mm = &init_mm,		\
	.tasks = LIST_HEAD_INIT(tsk.tasks),\
}

#define __init_task_data __attribute__((__section__(".data..init_task")))
#endif
