#ifndef __LINUX_MC146818RTC_H_
#define __LINUX_MC146818RTC_H_


#include <asm/mc146818rtc.h>

#define RTC_SECONDS 0
#define RTC_SECONDS_ALARM 1
#define RTC_MINUTES 2
#define RTC_MINUTES_ALARM 3
#define RTC_HOURS 4
#define RTC_HOURS_ALARM 5

#define RTC_DAY_OF_WEEK 6
#define RTC_DAY_OF_MONTH 7
#define RTC_MONTH 8
#define RTC_YEAR 9


#define RTC_REG_A 10
#define RTC_REG_B 11
#define RTC_REG_C 12
#define RTC_REG_D 13

#define RTC_FREQ_SELECT RTC_REG_A
#define RTC_CONTROL RTC_REG_B

#define RTC_UIP 0x80
#define RTC_DM_BINARY 0x04

#define BCD_TO_BIN(val) ((val) = ((val)&0xF) + ((val)>>4)*10)

#endif
