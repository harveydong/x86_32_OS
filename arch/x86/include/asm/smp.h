#ifndef __SMP_H_
#define __SMP_H_
#ifndef __ASSEMBLY__

extern void smp_alloc_memory(void);
#define NO_PROC_ID 0xFF

//#define smp_processor_id() (current->processor)


#endif

#endif
