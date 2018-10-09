#include <linux/init.h>
#include <linux/sched.h>
#include <linux/time.h>
#include <linux/mc146818rtc.h>
#include <asm/signal.h>
#include <linux/types.h>
#include <asm/errno.h>
#include <linux/interrupt.h>
#include <asm/msr.h>
#include <asm/io.h>
#include <linux/timex.h>

#include <linux/spinlock.h>

unsigned long fast_gettimeoffset_quotient;
extern int x86_udelay_tsc;
extern rwlock_t xtime_lock;

static unsigned long last_tsc_low;
unsigned long cpu_khz;
static int delay_at_last_interrupt;
static int use_tsc;

#define TICK_SIZE tick
spinlock_t i8253_lock = SPIN_LOCK_UNLOCKED;
extern spinlock_t i8259A_lock;

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

static long last_rtc_update;
int timer_ack;

static inline void do_timer_interrupt(int irq,void *dev_id,struct pt_regs *regs)
{
	if(timer_ack){
		spin_lock(&i8259A_lock);
		outb(0x0c,0x20);
		inb(0x20);
		spin_unlock(&i8259A_lock);
	}

	do_timer(regs);
	
	if((time_status & STA_UNSYNC) ==0 && xtime.tv_sec > last_rtc_update + 660 && xtime.tv_usec >= 500000 - ((unsigned)tick)/2 && xtime.tv_usec <= 500000 + ((unsigned) tick)/2) {
		last_rtc_update = xtime.tv_sec - 600;
	}

}

static void timer_interrupt(int irq,void *dev_id,struct pt_regs *regs)
{

	int count;
	write_lock(&xtime_lock);
	printk("into timer interrupt\n");

	if(use_tsc){
		rdtscl(last_tsc_low);
		spin_lock(&i8253_lock);
		outb_p(0x00,0x43);
		
		count = inb_p(0x40);
		count |= inb(0x40) << 8;
		spin_unlock(&i8253_lock);
		
		count = ((LATCH - 1) -count) *TICK_SIZE;
		delay_at_last_interrupt = (count + LATCH/2)/LATCH;
		
	}

	do_timer_interrupt(irq,NULL,regs);
	write_unlock(&xtime_lock);
}

static struct irqaction irq0 = {
	.handler = timer_interrupt ,
	.flags = SA_INTERRUPT,
	.mask = 0,
	.name = "timer",
	.dev_id = NULL,
	.next = NULL

}; 

#define CALIBRATE_LATCH (5*LATCH)
#define CALIBRATE_TIME (5*1000020/HZ)




static unsigned long __init calibrate_tsc(void)
{
	outb((inb(0x61) & ~0x02) | 0x01,0x61);
	outb(0xb0,0x43);
	outb(CALIBRATE_LATCH & 0xff,0x42);
	outb(CALIBRATE_LATCH >> 8,0x42);

	{
		unsigned long startlow,starthigh;
		unsigned long endlow,endhigh;
		unsigned long count;
		
		rdtsc(startlow,starthigh);
		count = 0;
		do{
			count++;
		}while((inb(0x61) & 0x20) == 0);

		rdtsc(endlow,endhigh);

		last_tsc_low = endlow;
		if(count <= 1)
			goto bad_ctc;
		
		__asm__("subl %2,%0\n\t"
			"sbbl %3,%1"
			:"=a"(endlow),"=d"(endhigh)
			:"g"(startlow),"g"(starthigh),"0"(endlow),"1"(endhigh));

		if(endhigh)
			goto bad_ctc;

		if(endlow <= CALIBRATE_TIME)
			goto  bad_ctc;

		__asm__("divl %2"
			:"=a"(endlow),"=d"(endhigh)
			:"r"(endlow),"0"(0),"1"(CALIBRATE_TIME));

		return endlow;

	}	
bad_ctc:
	

	return 0;
	


}

static void do_normal_gettime(struct timeval *tm)
{
	*tm = xtime;
}

void (*do_get_fast_time)(struct timeval *) = do_normal_gettime;



static inline unsigned long do_fast_gettimeoffset(void)
{
	register unsigned long eax,edx;
	rdtsc(eax,edx);

	eax -= last_tsc_low;

	__asm__("mull %2"
		:"=a"(eax),"=d"(edx)
		:"rm"(fast_gettimeoffset_quotient),"0"(eax));

	return delay_at_last_interrupt+edx;
}




#define do_gettimeoffset() do_fast_gettimeoffset()




static void do_gettimeofday(struct timeval *tv)
{
	printk("into do gettimeofday\n");
}
void __init time_init(void)
{
	extern int x86_udelay_tsc;

	xtime.tv_sec = get_cmos_time();
	xtime.tv_usec = 0;


	{
		unsigned long tsc_quotient =  calibrate_tsc();
		
		if(tsc_quotient){
			fast_gettimeoffset_quotient = tsc_quotient;
			use_tsc = 1;
			x86_udelay_tsc = 1;
			
		//	do_gettimeoffset = do_fast_gettimeoffset;
			do_get_fast_time = do_gettimeofday;
			{
				unsigned long eax = 0,edx = 1000;
				printk("tsc_quotient is %d\n",tsc_quotient);
				__asm__ ("divl %2":"=a"(cpu_khz),"=d"(edx):"r"(tsc_quotient),"0"(eax),"1"(edx));
				printk("detected %lu.%031u MHz processor.\n",cpu_khz/1000,cpu_khz%1000);
			}
		}
	}

	setup_irq(0,&irq0);

	printk("time init done!\n");
}
