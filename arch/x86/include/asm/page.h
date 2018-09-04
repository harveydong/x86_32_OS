#ifndef __PAGE_H_
#define __PAGE_H_

#ifndef __ASSEMBLY__
typedef struct { unsigned long pte_low,pte_high;} pte_t;
typedef struct { unsigned long long pmd;}pmd_t;
typedef struct { unsigned long long pgd;}pgd_t;



#endif

#define __PAGE_OFFSET (0xc0000000)
#define PAGE_OFFSET ((unsigned long)__PAGE_OFFSET)

#define __pa(x) ((unsigned long)(x) - PAGE_OFFSET)
#define __va(x) ((void *)(unsigned long)(x) + PAGE_OFFSET)

#endif
