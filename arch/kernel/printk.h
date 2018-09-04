#ifndef __PRINTK_H_
#define __PRINTK_H_
#include "types.h"

asmlinkage void early_printk(const char *fmt, ...);
#endif
