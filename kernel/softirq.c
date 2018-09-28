#include <linux/irq_cpustat.h>
#include <asm/hardirq.h>
#include <linux/threads.h>
#include <linux/linkage.h>
#include <asm/system.h>


irq_cpustat_t irq_stat[NR_CPUS];


static void(*bh_base[32])(void);

void init_bh(int nr,void (*routine)(void))
{
	bh_base[nr] = routine;
	mb();
}

asmlinkage void do_softirq()
{
	
}
