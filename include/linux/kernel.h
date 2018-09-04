#ifndef __KERNEL_H_
#define __KERNEL_H_
#include <linux/init.h>


asmlinkage int printk(const char *fmt,...) __attribute__((format(printf,1,2)));
extern int console_loglevel;


#endif
