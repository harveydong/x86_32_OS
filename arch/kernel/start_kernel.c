#include "init.h"
#include "types.h"
#include "stackprotector.h"
#include "printk.h"
#include "current.h"
#include "irqflags.h"
#include "smp.h"
#if 0
static void __init boot_cpu_init(void)
{
	int cpu = smp_processor_id();
	
	set_cpu_online(cpu,true);
//	set_cpu_active(cpu,true);
//	set_cpu_present(cpu,true);
//	set_cpu_possible(cpu,true);
}

#endif
asmlinkage void  __init start_kernel(void)
{
	boot_init_stack_canary();
//	early_printk("current stack is 0x%p\n",current->stack_canary);
	
	local_irq_disable();
//	boot_cpu_init();
	early_printk("ok I am now start kernel ....\n\n\n");
	while(1);
}
