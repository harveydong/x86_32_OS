#ifndef __SEMAPHORE_H_
#define __SEMAPHORE_H_
#include <asm/atomic.h>
#include <linux/wait.h>


struct semaphore{
	atomic_t count;
	int sleepers;
	wait_queue_head_t wait;

};
#define __SEMAPHORE_INITIALIZER(name,count) \
{ ATOMIC_INIT(count),0,__WAIT_QUEUE_HEAD_INITIALIZER((name).wait)}

#define __MUTEX_INITIALIZER(name) __SEMAPHORE_INITIALIZER(name,1)

static inline void sema_init(struct semaphore *sem,int val)
{
	atomic_set(&sem->count,val);
	sem->sleepers = 0;
	init_waitqueue_head(&sem->wait);

}

static inline void init_MUTEX(struct semaphore *sem)
{
	sema_init(sem,1);

}
#endif
