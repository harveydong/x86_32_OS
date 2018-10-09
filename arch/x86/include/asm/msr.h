#ifndef __ASM_MSR_H_
#define __ASM_MSR_H_



#define rdtsc(low,high) \
	__asm__ volatile("rdtsc":"=a"(low),"=d"(high))

#define rdtscl(low) \
	__asm__ volatile("rdtsc":"=a"(low)::"edx")

#endif
