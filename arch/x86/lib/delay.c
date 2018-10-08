#include <asm/param.h>
#include <asm/processor.h>
#include <linux/sched.h>

int x86_udelay_tsc = 0;

static void __loop_delay(unsigned long loops)
{
	int d0;
	__asm__ __volatile__(
		"\t jmp 1f \n"
		".align 16 \n"
		"1:\t jmp 2f \n"
		".align 16 \n"
		"2:\t decl %0\n\t jns 2b"
		:"=&a"(d0)
		:"0"(loops));

}
void __delay(unsigned long loops)
{
	__loop_delay(loops);
}
static inline void __const_udelay(unsigned long xloops)
{
	int d0;
	
	__asm__("mull %0"
		:"=d"(xloops),"=&a"(d0)
		:"1"(xloops),"0"(current_cpu_data.loops_per_jiffy));
	__delay(xloops*	HZ);
}
void __udelay(unsigned long usecs)
{
	__const_udelay(usecs*0x000010c6);
}
