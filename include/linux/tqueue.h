#ifndef __LINUX_TQUEUE_H_
#define __LINUX_TQUEUE_H_

#include <linux/list.h>

struct tq_struct {
	struct list_head list;
	unsigned long sync;
	void (*routing)(void*);
	void *data;

};

typedef struct list_head task_queue;

#define TQ_ACTIVE(q) (!list_empty(&q))
#define DECLARE_TASK_QUEUE(q) LIST_HEAD(q)


extern task_queue tq_timer;

static inline void run_task_queue(task_queue *list)
{

}
#endif
