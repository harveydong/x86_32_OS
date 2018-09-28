#ifndef __ASM_SPINLOCK_TYPES_H_
#define __ASM_SPINLOCK_TYPES_H_


typedef struct {

	volatile unsigned int lock;
}spinlock_t;

#define spin_is_locked(x) (*(volatile char*)(&(x)->lock) <=0)

#define spin_lock_string \
	"\n1:\t"\	
	"lock; decb %0\n\t" \
	"js 2f\n" \
	".section .text.lock,\"ax\"\n" \
	"2:\t"\
	"cmpb $0,%0\n\t" \
	"rep; nop\n\t" \
	"jle 2b\n\t"\
	"jmp 1b\n"\
	".previous"


static inline void spin_lock(spinlock_t *lock)
{
	__asm__ volatile(
		spin_lock_string
		:"=m"(lock->lock)::"memory");
}

#define spin_unlock_string \
	"movb $1,%0"

static inline void spin_unlock(spinlock_t *lock)
{
	__asm__ volatile(
		spin_unlock_string
		:"=m"(lock->lock)::"memory");
}
#endif
