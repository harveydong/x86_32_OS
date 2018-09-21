#ifndef __SMP_H_
#define __SMP_H_
#ifndef __ASSEMBLY__
#include <asm/current.h>

extern void smp_alloc_memory(void);

#define smp_processor_id() (current->processor)

#endif

#endif
