#ifndef __LINUX_TIMEX_H_
#define __LINUX_TIMEX_H_
#include <asm/timex.h>
#include <asm/param.h>

#define LATCH ((CLOCK_TICK_RATE + HZ/2)/HZ)
#endif
