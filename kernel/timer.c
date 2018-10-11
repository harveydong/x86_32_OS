#include <linux/list.h>
#include <linux/printk.h>
#include <linux/sched.h>
#include <linux/time.h>
#include <linux/spinlock.h>
#include <linux/tqueue.h>
#include <linux/interrupt.h>
#include <asm/param.h>
#include <linux/timex.h>

#define TVN_BITS 6
#define TVR_BITS 8

#define TVN_SIZE (1 << TVN_BITS)
#define TVR_SIZE (1 << TVR_BITS)

long tick = (1000000 +HZ/2)/HZ;
volatile struct timeval xtime __attribute__((aligned(16)));


unsigned long volatile jiffies;


DECLARE_TASK_QUEUE(tq_timer);

int time_status = STA_UNSYNC;


void tqueue_bh(void)
{
	run_task_queue(&tq_timer);

}
struct timer_vec{
	int index;
	struct list_head vec[TVN_SIZE];
};

struct timer_vec_root{
	int index;
	struct list_head vec[TVR_SIZE];
};


static struct timer_vec tv5;
static struct timer_vec tv4;
static struct timer_vec tv3;
static struct timer_vec tv2;
static struct timer_vec_root tv1;

static struct timer_vec *const tvecs[] = {
	[0] = (struct timer_vec*)&tv1,
	[1] = &tv2,
	[2] = &tv3,
	[3] = &tv4,
	[4] = &tv5
};


void init_timervecs(void)
{
	int i;

	for(i = 0; i < TVN_SIZE;i++){

		INIT_LIST_HEAD(tv5.vec + i);
		INIT_LIST_HEAD(tv4.vec + i);
		INIT_LIST_HEAD(tv3.vec + i);
		INIT_LIST_HEAD(tv2.vec + i);
	}

	for(i = 0;i < TVR_SIZE;i++)
		INIT_LIST_HEAD(tv1.vec + i);

}

static void update_wall_time(unsigned long ticsk)
{

}


unsigned long wall_jiffies;

rwlock_t xtime_lock = RW_LOCK_UNLOCKED;
static inline void update_times(void)
{
	unsigned long ticks;
	
	write_lock_irq(&xtime_lock);
	
	ticks = jiffies - wall_jiffies;
	if(ticks){
		wall_jiffies += ticks;
		update_wall_time(ticks);
	
	}

	write_unlock_irq(&xtime_lock);
//	calc_load(ticks);
}

static inline void run_timer_list(void)
{

}

void timer_bh(void)
{
	update_times();
	run_timer_list();	
}


void update_process_times(int user_tick)
{
	struct task_struct *p = current;
	int cpu = smp_processor_id(),system = user_tick ^ 1;
		
	printk("into update process uptimes\n");
}





void do_timer(struct pt_regs *regs)
{
	(*(unsigned long *)&jiffies)++;
	printk("do timer and jiffies is 0x%x\n",jiffies);	
	mark_bh(TIMER_BH);
	if(TQ_ACTIVE(tq_timer))
		mark_bh(TQUEUE_BH);
}
