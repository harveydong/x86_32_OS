#ifndef __ATOMIC_H_
#define __ATOMIC_H_

typedef struct{
	volatile int counter;
}atomic_t;

#define ATOMIC_INIT(i) {(i)}
#define atomic_set(v,i) (((v)->counter)=(i))
#define atomic_read(v) ((v)->counter)


#define LOCK "lock;"



static inline void atomic_inc(atomic_t *v)
{
	__asm__ __volatile__(

	LOCK"incl %0"
	:"=m"(v->counter)
	:"m"(v->counter)
	);
}


static inline int atomic_dec_and_test(atomic_t *v)
{
	unsigned char c;
	__asm__ __volatile__(
		LOCK"decl %0;sete %1"
		:"=m"(v->counter),"=qm"(c)
		:"m"(v->counter):"memory");

	return c != 0;
}
#endif
