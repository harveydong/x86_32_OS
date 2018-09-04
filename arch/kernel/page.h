#ifndef __PAGE_H_
#define __PAGE_H_
#include "page_64_types.h"
#include "physaddr.h"

#define __pa(x)	__phys_addr((unsigned long)(x))
#define __pa_symbol(x) __pa(__phys_reloc_hide((unsigned long)(x)))
#define __va(x) ((void *)((unsigned long)(x) + PAGE_OFFSET))
#endif
