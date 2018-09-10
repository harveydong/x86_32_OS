#ifndef __PAGE_H_
#define __PAGE_H_

#ifndef __ASSEMBLY__
#include <linux/printk.h>

typedef struct { unsigned long pte_low,pte_high;} pte_t;
typedef struct { unsigned long long pmd;}pmd_t;
typedef struct { unsigned long long pgd;}pgd_t;


#define BUG() do{\
	printk("kernel BUG at %s:%d!\n",__FILE__,__LINE__);\
	__asm__ __volatile__(".byte 0x0f,0x0b");\
}while(0)
#endif

#define PAGE_SHIFT 12
#define PAGE_SIZE (1UL << PAGE_SHIFT)
#define PAGE_MASK (~(PAGE_SIZE - 1))

#define __PAGE_OFFSET (0xc0000000)
#define PAGE_OFFSET ((unsigned long)__PAGE_OFFSET)

#define __pa(x) ((unsigned long)(x) - PAGE_OFFSET)
#define __va(x) ((void *)(unsigned long)(x) + PAGE_OFFSET)

#endif
