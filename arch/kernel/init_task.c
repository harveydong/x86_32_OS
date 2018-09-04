#include "thread_info.h"
#include "sched.h"
#include "init_task.h"

struct task_struct init_task = INIT_TASK(init_task);


union thread_union init_thread_union __init_task_data = {
	INIT_THREAD_INFO(init_task)
};
