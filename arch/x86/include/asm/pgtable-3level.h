#ifndef __PGTABLE_3LEVEL_H_
#define __PGTABLE_3LEVEL_H_
#include <asm/page.h>
#include <asm/pgtable.h>
#include <asm/system.h>

#define EMPTY_PGD (__pa(empty_zero_page) + 1)
#define pgd_none(x) (pgd_val(x) == EMPTY_PGD)

#define set_pmd(pmdptr,pmdval)\
	set_64bit((unsigned long *)(pmdptr),pmd_val(pmdval))

#define set_pgd(pgdptr,pgdval)\
	set_64bit((unsigned long*)(pgdptr),pgd_val(pgdval))

#define PGDIR_SHIFT 22
#define PTRS_PER_PGD 1024

#define PMD_SHIFT 
#define PTRS_PER_PMD 1024

#define PTRS_PER_PTE 512



extern inline void __pgd_clear(pgd_t *pgd)
{
	set_pgd(pgd,__pgd(EMPTY_PGD));
}


#define pgd_page(pgd) ((unsigned long) __va(pgd_val(pgd) &PAGE_MASK))

#define pmd_offset(dir,address) ((pmd_t*)pgd_page(*(dir)) + __pmd_offset(address))


static inline pte_t __mk_pte(unsigned long page_nr,pgprot_t pgprot)
{
	pte_t pte;
	
	pte.pte_high = page_nr >> (32 - PAGE_SHIFT);
	pte.pte_low = (page_nr << PAGE_SHIFT) | pgprot_val(pgprot);

	return pte;
}


#endif
