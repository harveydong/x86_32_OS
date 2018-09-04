#ifndef __CPUMASK_H_
#define __CPUMASK_H_

#define to_cpumask(bitmap)	\
	((struct cpumask *)(1?(bitmap)	\
		:	(void*)sizeof(__check_is_bitmap(bitmap))))
#endif
