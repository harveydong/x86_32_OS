#ifndef __HARDIRQ_H_
#define __HARDIRQ_H_
#include <linux/cache.h>
#include <asm/bitops.h>
#include <linux/irq_cpustat.h>


extern unsigned volatile long global_irq_lock;


static inline void irq_enter(int cpu,int irq)
{
	++local_irq_count(cpu);
	
	while(test_bit(0,&global_irq_lock)){
		
	}
}

static inline void irq_exit(int cpu,int irq)
{
	--local_irq_count(cpu);
}

#endif
