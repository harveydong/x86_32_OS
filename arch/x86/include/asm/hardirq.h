#ifndef __HARDIRQ_H_
#define __HARDIRQ_H_
#include <linux/cache.h>
#include <asm/bitops.h>
#include <linux/irq_cpustat.h>
#include <linux/smp.h>
#include <asm/smp.h>

extern unsigned volatile long global_irq_lock;
extern unsigned char global_irq_holder;

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

static inline int irqs_running(void)
{
	int i;
	
	for(i = 0; i < smp_num_cpus;i++)
		if(local_irq_count(i))
			return 1;

	return 0;
}


static inline void release_irqlock(int cpu)
{
	if(global_irq_holder == (unsigned char)cpu){
		global_irq_holder = NO_PROC_ID;
		clear_bit(0,&global_irq_lock);
	}
}


static inline int hardirq_trylock(int cpu)
{
	return !local_irq_count(cpu) && !test_bit(0,&global_irq_lock);
}

#define hardirq_endlock(cpu)	do { } while (0)

#endif
