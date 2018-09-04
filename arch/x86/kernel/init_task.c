#include <linux/sched.h>
#include <linux/mm.h>


static struct vm_area_struct init_mmap = INIT_MMAP;

union task_union init_task_union __attribute__((__section__(".data.init_task"))) = {INIT_TASK(init_task_union.task)};


struct mm_struct init_mm = INIT_MM(init_mm);
