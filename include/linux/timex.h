#ifndef __LINUX_TIMEX_H_
#define __LINUX_TIMEX_H_
#include <asm/timex.h>
#include <asm/param.h>

#define LATCH ((CLOCK_TICK_RATE + HZ/2)/HZ)

extern long tick;
extern int time_status;

#define STA_UNSYNC 0x0040
#endif
