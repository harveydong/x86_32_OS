#ifndef __EARLY_PRINTK_
#define __EARLY_PRINTK_

#include <linux/linkage.h>
asmlinkage void early_printk(const char*fmt,...);
#endif
