#ifndef __LINUX_TIME_H_
#define __LINUX_TIME_H_
#include <linux/types.h>
#include <asm/ptrace.h>
struct timeval{

	time_t tv_sec;
	suseconds_t tv_usec;
	
};



extern volatile struct timeval xtime ;
extern void do_timer(struct pt_regs*);

static inline unsigned long mktime(unsigned int year,unsigned int mon,unsigned int day,unsigned int hour,unsigned int min,unsigned int sec)
{
	if(0 >= (int)(mon -= 2)){
		mon += 12;
		year -= 1;
	}

	return ((((unsigned long)(year/4 - year/100 + year/400 + 367*mon/12 + day) + year*365 - 719499)*24 + hour)*60 + min)*60 + sec;

}
#endif
