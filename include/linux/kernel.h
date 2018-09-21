#ifndef __KERNEL_H_
#define __KERNEL_H_
#include <linux/init.h>
#include <linux/linkage.h>

#define NORET_TYPE

asmlinkage int printk(const char *fmt,...) __attribute__((format(printf,1,2)));
extern int console_loglevel;

NORET_TYPE void do_exit(long code);


#endif
