#ifndef __SYSTEM_H_
#define __SYSTEM_H_

#define __restore_flags(x) _asm__ __volatile__("pushl %0;popfl"::"g"(x):"memory")

#define local_irq_save(x) __asm__ __volatile__("pushl;popl %0;cli":"=g"(x)::"memory")
#define local_irq_restore(x) __restore_flags(x)
#endif
