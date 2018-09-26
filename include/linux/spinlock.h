#ifndef __SPINLOCK_H_
#define __SPINLOCK_H_
#include <asm/rwlock.h>
#include <asm/spinlock.h>
#include <asm/system.h>

#define SPINLOCK_MAGIC_INIT
#define SPIN_LOCK_UNLOCKED (spinlock_t){1 SPINLOCK_MAGIC_INIT}

#define spin_lock_irqsave(lock,flags) do{local_irq_save(flags);spin_lock(lock);}while(0)
#define spin_unlock_irqrestore(lock,flags) do{spin_unlock(lock); local_irq_restore(flags);}while(0)


typedef struct {
	volatile unsigned int lock;

}rwlock_t;

#define RW_LOCK_UNLOCKED (rwlock_t){.lock = RW_LOCK_BIAS}


static inline void write_lock(rwlock_t *rw)
{
	__build_write_lock(rw,"__write_lock_failed");
}

#define write_unlock(rw) asm volatile("lock;addl $"RW_LOCK_BIAS_STR ",%0":"=m"((rw)->lock)::"memory")



#endif
