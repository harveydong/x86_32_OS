#ifndef __ATOMIC_H_
#define __ATOMIC_H_

typedef struct{
	volatile int counter;
}atomic_t;

#define ATOMIC_INIT(i) {(i)}
#define atomic_set(v,i) (((v)->counter)=(i))


#define LOCK "lock;"



static inline void atomic_inc(atomic_t *v)
{
	__asm__ __volatile__(

	LOCK"incl %0"
	:"=m"(v->counter)
	:"m"(v->counter)
	);
}
#endif
