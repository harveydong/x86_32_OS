#ifndef __KERNEL_H_
#define __KERNEL_H_

#define roundup(x,y) ((((x) +((y) - 1)) / (y)) * (y))
int __init setup_early_printk(void);

#endif
