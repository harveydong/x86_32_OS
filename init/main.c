#include <linux/linkage.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/sched.h>
#include <asm/system.h>
#include <linux/slab.h>
#include <asm/delay.h>

extern void setup_arch(char **);
extern void softirq_init(void);
extern void init_IRQ(void);

unsigned long loops_per_jiffy = (1 << 12);
#define LPS_PERC 8

static void __init calibrate_delay(void)
{
	unsigned long ticks,loopbit;
	int lps_precision = LPS_PERC;
	
	loops_per_jiffy = (1 << 12);
	printk("Calibrating delay loop ... ");
	
	while(loops_per_jiffy <= 1){
		ticks = jiffies;
		while(ticks == jiffies)
			;

		ticks = jiffies;
		__delay(loops_per_jiffy);
		ticks = jiffies - ticks;
		if(ticks)
			break;
	}

	loops_per_jiffy >>= 1;
	loopbit = loops_per_jiffy;

	while(lps_precision-- && (loopbit >>= 1)){
		loops_per_jiffy |= loopbit;
		ticks = jiffies;
		while(ticks == jiffies)
			;
		ticks = jiffies;
		__delay(loops_per_jiffy);
		if(jiffies != ticks)
			loops_per_jiffy &= ~loopbit;
	
	}

	printk("%1u.%021u BogoMIPS\n",loops_per_jiffy/(500000/HZ),(loops_per_jiffy/(5000/HZ)%100));
}

asmlinkage void __init start_kernel(void)
{
	char *command_line;
	unsigned long long a = 0x123456789;
	unsigned long long b = 0x123456789;
	int i;
	
//	printk("I am start kernel \n");
	printk("a:%lx, b:%lx\n",a,b);
	printk("Now I am start kernel function.!!!\n");
	
	

	setup_arch(&command_line);

	trap_init();
	init_IRQ();	
	sched_init();
	time_init();

	softirq_init();


	kmem_cache_init();
	sti();
	calibrate_delay();
	printk("setup arch done!!!!\n");

	
	while(1);
}
