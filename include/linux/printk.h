#ifndef __PRINTK_H_
#define __PRINTK_H_
#include <linux/linkage.h>

asmlinkage int printk(const char *fmt, ...);

#endif
