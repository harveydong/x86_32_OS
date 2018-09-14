#ifndef __HIGHMEM_H_
#define __HIGHMEM_H_
#include <asm/page.h>

extern unsigned long highstart_pfn,highend_pfn;

#define PKMAP_BASE (0xfe000000UL)
#define LAST_PKMAP 512

extern pte_t *pkmap_page_table;
#endif
