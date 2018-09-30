#include <linux/list.h>
#include <linux/printk.h>
#include <linux/sched.h>
#include <linux/time.h>

#define TVN_BITS 6
#define TVR_BITS 8

#define TVN_SIZE (1 << TVN_BITS)
#define TVR_SIZE (1 << TVR_BITS)


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


void timer_bh(void)
{
	
}


void update_process_times(int user_tick)
{
	struct task_struct *p = current;
	int cpu = smp_processor_id(),system = user_tick ^ 1;
		
	printk("into update process uptimes\n");
}



volatile struct timeval xtime __attribute__((aligned(16)));

