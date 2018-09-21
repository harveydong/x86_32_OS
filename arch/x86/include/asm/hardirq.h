#ifndef __HARDIRQ_H_
#define __HARDIRQ_H_
#include <linux/cache.h>

typedef struct{
	unsigned int __softirq_active;
	unsigned int __softirq_mask;
	unsigned int __local_irq_count;
	unsigned int __local_bh_count;
	unsigned int __syscall_count;
	unsigned int __nmi_count;
}____cacheline_aligned irq_cpustat_t;
#endif
