#ifndef __SPINLOCK_H_
#define __SPINLOCK_H_


typedef struct{
	volatile unsigned long lock;
	volatile unsigned long owner_pc;
	volatile unsigned long owner_cpu;
}spinlock_t;

#define SPINLOCK_MAGIC_INIT
#define SPIN_LOCK_UNLOCKED (spinlock_t){1 SPINLOCK_MAGIC_INIT}

#define spin_lock_irqsave(lock,flags) do{local_irq_save(flags);spin_lock(lock);}while(0)


#endif
