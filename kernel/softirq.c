#include <linux/irq_cpustat.h>
#include <asm/hardirq.h>
#include <linux/threads.h>
#include <linux/linkage.h>
#include <asm/system.h>
#include <linux/spinlock.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <asm/bitops.h>

irq_cpustat_t irq_stat[NR_CPUS];
spinlock_t global_bh_lock = SPIN_LOCK_UNLOCKED;

static void(*bh_base[32])(void);
static struct softirq_action softirq_vec[32];


void init_bh(int nr,void (*routine)(void))
{
	bh_base[nr] = routine;
	mb();
}

asmlinkage void do_softirq()
{
	
}
static spinlock_t softirq_mask_lock = SPIN_LOCK_UNLOCKED;


static void open_softirq(int nr,void (*action)(struct softirq_action*),void *data)
{
	unsigned long flags;
	int i;

	spin_lock_irqsave(&softirq_mask_lock,flags);
	softirq_vec[nr].data = data;
	softirq_vec[nr].action = action;

	for(i = 0;i < NR_CPUS;i++)
		softirq_mask(i) |= (1<<nr);

	spin_unlock_irqrestore(&softirq_mask_lock,flags);
	
}

struct tasklet_struct bh_task_vec[32];
static void tasklet_init(struct tasklet_struct *t,void (*func)(unsigned long),unsigned long data)
{

	t->func = func;
	t->data = data;
	t->state = 0;
	atomic_set(&t->count,0);

}



static void bh_action(unsigned long nr)
{
	int cpu = smp_processor_id();

	if(!spin_trylock(&global_bh_lock));
		goto resched;
	if(!hardirq_trylock(cpu))
		goto resched_unlock;

	if(bh_base[nr])
		bh_base[nr]();

	hardirq_endlock(cpu);
	spin_unlock(&global_bh_lock);
	return;

resched_unlock:
	spin_unlock(&global_bh_lock);
resched:

	mark_bh(nr);
}

struct tasklet_head tasklet_vec[NR_CPUS];

static void tasklet_action(struct softirq_action *a)
{
int cpu = smp_processor_id();
	struct tasklet_struct *list;

	local_irq_disable();
	list = tasklet_vec[cpu].list;

	tasklet_vec[cpu].list = NULL;
	local_irq_enable();

	while(list != NULL){
		struct tasklet_struct *t = list;
		list = list->next;
		
		if(tasklet_trylock(t)){
			if(atomic_read(&t->count) == 0){
				clear_bit(TASKLET_STATE_SCHED,&t->state);
				t->func(t->data);
				smp_mb_before_clear_bit();
				tasklet_unlock(t);
				continue;
			}

			tasklet_unlock(t);
		}

		local_irq_disable();
		t->next = tasklet_vec[cpu].list;
		tasklet_vec[cpu].list = t;
		__cpu_raise_softirq(cpu,TASKLET_SOFTIRQ);
		local_irq_enable();
	}

}


struct tasklet_head tasklet_hi_vec[NR_CPUS];
static void tasklet_hi_action(struct softirq_action *a)
{
	int cpu = smp_processor_id();
	struct tasklet_struct *list;

	local_irq_disable();
	list = tasklet_hi_vec[cpu].list;
	tasklet_hi_vec[cpu].list = NULL;
	local_irq_enable();

	while(list != NULL){
		struct tasklet_struct *t = list;
		list = list->next;
		
		if(tasklet_trylock(t)){
			if(atomic_read(&t->count) == 0){

				clear_bit(TASKLET_STATE_SCHED,&t->state);
				t->func(t->data);
				tasklet_unlock(t);
				continue;
			}
			tasklet_unlock(t);
		}

		local_irq_disable();
		t->next = tasklet_hi_vec[cpu].list;
		tasklet_hi_vec[cpu].list = t;
		__cpu_raise_softirq(cpu,HI_SOFTIRQ);
		local_irq_enable();
	}

}

void __init softirq_init(void)
{

	int i;

	for(i = 0; i < 32; i++)
		tasklet_init(bh_task_vec + i,bh_action,i);

	open_softirq(TASKLET_SOFTIRQ,tasklet_action,NULL);
	open_softirq(HI_SOFTIRQ,tasklet_hi_action,NULL);	

	printk("softirq init done!\n");
}
