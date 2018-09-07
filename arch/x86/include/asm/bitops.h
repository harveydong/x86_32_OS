#ifndef __BITOPS_H_
#define __BITOPS_H_

#define LOCK_PREFIX "lock ;"
#define ADDR (*(volatile long*)addr)



static inline int test_and_set_bit(int nr,volatile void *addr)
{
	int oldbit;

	__asm__ __volatile__(LOCK_PREFIX
		"btsl %2,%1\n\tsbbl %0,%0"
		:"=r"(oldbit),"=m"(ADDR)
		:"Ir"(nr):"memory");
	return oldbit;

}
static inline int test_and_clear_bit(int nr,volatile void *addr)
{
	int oldbit;
	
	__asm__ __volatile__(LOCK_PREFIX
		"btrl %2,%1\n\tsbbl %0,%0"
		:"=r"(oldbit),"=m"(ADDR)
		:"Ir"(nr):"memory");

	return oldbit;
}


static inline int constant_test_bit(int nr,const volatile void *addr)
{
	return ((1UL << (nr & 31)) & (((const volatile unsigned int*)addr)[nr >> 5])) != 0;
}

static inline int variable_test_bit(int nr,volatile void *addr)
{
	int oldbit;
	
	__asm__ __volatile__(
		"btl %2,%1\n\tsbbl %0,%0"
		:"=r"(oldbit)
		:"m"(ADDR),"Ir"(nr));

	return oldbit;
}

#define test_bit(nr,addr)\
(__builtin_constant_p(nr)?\
	constant_test_bit((nr),(addr)):\
	variable_test_bit((nr),(addr)))

#endif
