#ifndef __SYSTEM_H_
#define __SYSTEM_H_

#define __restore_flags(x) _asm__ __volatile__("pushl %0;popfl"::"g"(x):"memory")

#define local_irq_save(x) __asm__ __volatile__("pushl;popl %0;cli":"=g"(x)::"memory")
#define local_irq_restore(x) __restore_flags(x)


extern inline void __set_bit64(unsigned long *ptr,unsigned int low,unsigned int high)
{
	__asm__ volatile(
			"1:movl (%0),%%eax;movl 4(%0),%%edx;cmpxchg8b (%0);jnz 1b"
		::"D"(ptr),
		  "b"(low),	
		  "c"(high)
		:"ax","dx","memory");
}


#define ll_low(x) *(((unsigned int*)&(x))+0)
#define ll_high(x) *(((unsigned int*)&(x))+0)

extern void inline __set_64bit_var(unsigned long *ptr,unsigned long value)
{
	__set_bit64(ptr,ll_low(value),ll_high(value));
}

#define set_64bit(ptr,value) \
	__set_64bit_var(ptr,value)
#endif
