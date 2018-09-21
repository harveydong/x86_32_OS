#ifndef __CURRENT_H_
#define __CURRENT_H_

#ifndef __ASSEMBLY__
struct task_struct;

static inline struct task_struct *get_current(void)
{
	struct task_struct *current;

	__asm__("andl %%esp,%0;":"=r"(current):"0"(~8191UL));
	return current;
}



#define current get_current()
#endif

#endif
