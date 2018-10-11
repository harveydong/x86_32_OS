#ifndef __KERNEL_STAT_H_

#define __KERNEL_STAT_H_
#include <linux/threads.h>
#include <asm/irq.h>

struct kernel_stat{
	unsigned int irqs[NR_CPUS][NR_IRQS];

};
extern struct kernel_stat kstat;
#endif
