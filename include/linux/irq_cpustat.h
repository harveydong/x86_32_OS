#ifndef __IRQ_CPUSTAT_H_
#define __IRQ_CPUSTAT_H_
#include <linux/cache.h>

typedef struct{
	unsigned int __softirq_active;
	unsigned int __softirq_mask;
	unsigned int __local_irq_count;
	unsigned int __local_bh_count;
	unsigned int __syscall_count;
	unsigned int __nmi_count;
}____cacheline_aligned irq_cpustat_t;


extern irq_cpustat_t irq_stat[];

#define __IRQ_STAT(cpu,member) (irq_stat[cpu].member)

#define local_irq_count(cpu) __IRQ_STAT((cpu),__local_irq_count)
#endif
