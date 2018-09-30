#ifndef __ASMMC146818RTC_H_
#define __ASMMC146818RTC_H_
#include <asm/io.h>

#define RTC_PORT(x) (0x70+(x))
#define RTC_ALWAYS_BCD 1

#define CMOS_READ(addr) ({outb_p((addr),RTC_PORT(0));inb_p(RTC_PORT(1));})
#endif
