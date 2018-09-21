#ifndef __INTERRUPT_H_
#define __INTERRUPT_H_
#include <linux/linkage.h>

asmlinkage void do_softirq(void);

#endif
