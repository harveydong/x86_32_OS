#include "init.h"
#include "page_64_types.h"
#include "kernel.h"
#include "p_types.h"
#include "sections.h"
#include "mm_low_types.h"

void __init cleanup_highmap(void)
{
	unsigned long vaddr = __START_KERNEL_map;
	unsigned long end = roundup((unsigned long)_end,PMD_SIZE) -1;
	pmd_t *pmd = level2_kernel_pgt;
	pmd_t *last_pmd = pmd + PTRS_PER_PMD;
	
	for(; pmd < last_pmd; pmd++,vaddr += PMD_SIZE){
		if(pmd_none(*pmd))
			continue;
		
		if(vaddr < (unsigned long)_text || vaddr > end)
			set_pmd(pmd,__pmd(0));
	}
}
