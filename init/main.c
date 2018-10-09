#include <linux/linkage.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/sched.h>
#include <asm/system.h>


extern void setup_arch(char **);
extern void softirq_init(void);
extern void init_IRQ(void);

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
	printk("setup arch done!!!!\n");

	
	while(1);
}
