#include <linux/linkage.h>
#include <linux/init.h>
//#include <linux/printk.h>
#include <asm/early_printk.h>

asmlinkage void __init start_kernel(void)
{
//	printk("I am start kernel \n");
	early_printk("Now I am start kernel function.!!!\n");
	while(1);
}
