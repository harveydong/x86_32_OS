#ifndef __WAIT_H_
#define __WAIT_H_
#include <linux/spinlock.h>
#include <linux/list.h>


#define wq_lock_t spinlock_t

struct __wait_queue_head{
	wq_lock_t lock;
	struct list_head task_list;

};


typedef struct __wait_queue_head wait_queue_head_t;

#define WAITQUEUE_RW_LOCK_UNLOCKED SPIN_LOCK_UNLOCKED

#define __WAIT_QUEUE_HEAD_INITIALIZER(name)\
{WAITQUEUE_RW_LOCK_UNLOCKED,{&(name).task_list,&(name).task_list}}



static inline void init_waitqueue_head(wait_queue_head_t *q)
{
	q->lock = WAITQUEUE_RW_LOCK_UNLOCKED;
	
}
#endif
