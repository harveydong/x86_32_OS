#ifndef __PGTABLE_2LEVEL_H_
#define __PGTABLE_2LEVEL_H_
#include <asm/page.h>
#include <asm/pgtable.h>
#include <asm/system.h>

#define EMPTY_PGD (__pa(empty_zero_page) + 1)
#define pgd_none(x) (pgd_val(x) == EMPTY_PGD)

#define set_pmd(pmdptr,pmdval)\
	set_64bit((unsigned long *)(pmdptr),pmd_val(pmdval))

#define set_pgd(pgdptr,pgdval)\
	set_64bit((unsigned long*)(pgdptr),pgd_val(pgdval))



#define set_pte(pteptr,pteval) (*(pteptr) = pteval)

#define PGDIR_SHIFT 22
#define PTRS_PER_PGD 1024

#define PMD_SHIFT 22 
#define PTRS_PER_PMD 1

#define PTRS_PER_PTE 1024



extern inline void __pgd_clear(pgd_t *pgd)
{
	set_pgd(pgd,__pgd(EMPTY_PGD));
}


#define pgd_page(pgd) ((unsigned long) __va(pgd_val(pgd) &PAGE_MASK))

extern inline pmd_t *pmd_offset(pgd_t *dir,unsigned int address)
{
	return (pmd_t*)dir;
}


static inline pte_t __mk_pte(unsigned long page_nr,pgprot_t pgprot)
{
	pte_t pte;
	
	pte.pte_low = page_nr >> (32 - PAGE_SHIFT);
	pte.pte_low = (page_nr << PAGE_SHIFT) | pgprot_val(pgprot);

	return pte;
}


#endif
