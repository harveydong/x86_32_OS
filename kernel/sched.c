
#include <linux/linkage.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <asm/atomic.h>
#include <asm/mmu_context.h>
#include <asm/processor.h>
#include <linux/printk.h>
#include <linux/kernel_stat.h>

extern void init_timervecs(void);
extern void timer_bh(void);
extern void tqueue_bh(void);

struct kernel_stat kstat;


void __init sched_init(void)
{
	int cpu = smp_processor_id();
	int nr;

	init_task.processor = cpu;
	
	for(nr = 0; nr < PIDHASH_SZ;nr++)
		pidhash[nr] = NULL;	

	init_timervecs();

	init_bh(TIMER_BH,timer_bh);
	init_bh(TQUEUE_BH,tqueue_bh);

	atomic_inc(&init_mm.mm_count);

	enter_lazy_tlb(&init_mm,current,cpu);	

	printk("sched init ok\n");

}

asmlinkage void schedule(void)
{


}
