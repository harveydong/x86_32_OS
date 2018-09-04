#ifndef __STACKPROTECTOR_H_
#define __STACKPROTECTOR_H_
#include "init.h"
#include "types.h"
#include "msr_fun.h"
#include "percpu.h"
#include "current.h"
#include "sched.h"
#include "percpu_fun.h"
#include "printk.h"

extern union irq_stack_union irq_stack_union;

static __always_inline void boot_init_stack_canary(void)
{
	u64 canary;
	u64 tsc;

//	get_random_bytes(&canary,sizeof(canary));
	tsc = __native_read_tsc();		

	canary += tsc + (tsc << 32UL);
	current->stack_canary = canary;
	early_printk("canary is 0x%lx\n",canary);
	early_printk("current is %lx,and st_ca:0x%lx\n",current,current->stack_canary);	
//	percpu_write(irq_stack_union.stack_canary,canary);
}
#endif
