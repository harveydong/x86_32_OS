#ifndef __SYSTEM_H_
#define __SYSTEM_H_

#define __restore_flags(x) __asm__ __volatile__("pushl %0;popfl"::"g"(x):"memory")

#define local_irq_save(x) __asm__ __volatile__("pushfl;popl %0;cli":"=g"(x)::"memory")
#define local_irq_restore(x) __restore_flags(x)


static inline void __set_bit64(unsigned long *ptr,unsigned int low,unsigned int high)
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

static void inline __set_64bit_var(unsigned long *ptr,unsigned long value)
{
	__set_bit64(ptr,ll_low(value),ll_high(value));
}

#define set_64bit(ptr,value) \
	__set_64bit_var(ptr,value)

#define __sti() __asm__ __volatile__("sti":::"memory");


#define read_cr0() ({\
	unsigned int __dummy;\
	__asm__(\
	"movl %%cr0,%0\n\t"\
	:"=r"(__dummy));\
	__dummy;\
})

#define write_cr0(x) \
	__asm__("movl %0,%%cr0"::"r"(x))

#define stts() write_cr0(8|read_cr0())



#define mb() __asm__ __volatile__("sfence":::"memory")
#endif
