#include <linux/irq_cpustat.h>
#include <asm/hardirq.h>
#include <linux/threads.h>
#include <linux/linkage.h>


irq_cpustat_t irq_stat[NR_CPUS];

asmlinkage void do_softirq()
{
	
}
