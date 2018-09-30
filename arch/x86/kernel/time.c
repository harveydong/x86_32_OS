#include <linux/init.h>
#include <linux/sched.h>
#include <linux/time.h>
#include <linux/mc146818rtc.h>


unsigned long get_cmos_time(void)
{
	unsigned int year,mon,day,hour,min,sec;

	int i;

	for(i = 0; i < 1000000;i++)
		if(CMOS_READ(RTC_FREQ_SELECT) & RTC_UIP)
			break;

	for(i = 0; i < 1000000; i++)
		if(!(CMOS_READ(RTC_FREQ_SELECT)& RTC_UIP))
			break;

	do{
		sec = CMOS_READ(RTC_SECONDS);
		min = CMOS_READ(RTC_MINUTES);
		hour = CMOS_READ(RTC_HOURS);
		day = CMOS_READ(RTC_DAY_OF_MONTH);
		mon = CMOS_READ(RTC_MONTH);
		year = CMOS_READ(RTC_YEAR);
	}while(sec != CMOS_READ(RTC_SECONDS));	

	if(!(CMOS_READ(RTC_CONTROL) & RTC_DM_BINARY) || RTC_ALWAYS_BCD)
	{
		BCD_TO_BIN(sec);
		BCD_TO_BIN(min);
		BCD_TO_BIN(hour);
		BCD_TO_BIN(day);
		BCD_TO_BIN(mon);
		BCD_TO_BIN(year);
	}

	if((year += 1900) < 1970)
		year += 100;

	return mktime(year,mon,day,hour,min,sec);
	
}


static struct irqaction irq0 = 
void __init time_init(void)
{
	extern int x86_udelay_tsc;

	xtime.tv_sec = get_cmos_time();
	xtime.tv_usec = 0;

	{
		unsigned long tsc_quotient = calibrate_tsc();
		if(tsc_quotient){
			fast_gettimeoffset_quotient = tsc_quotient;
			use_tsc = 1;
			x86_udelay_tsc = 1;
			
			do_gettimeoffset = do_fast_gettimeoffset;
			do_get_fast_time = do_gettimeofday;
			{
				unsigned long eax = 0,edx = 1000;
				__asm__ ("divl %2":"=a"(cpu_khz),"=d"(edx):"r"(tsc_quotient),
				printk("detected %lu.%031u MHz processor.\n",cpu_khz/1000,cpu_khz%1000);
			}
		}
	}

	setup_irq(0,&irq0);
}
