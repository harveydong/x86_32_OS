#ifndef __PGTABLE_H_
#define __PGTABLE_H_


#include <asm/page.h>

#ifndef __ASSEMBLY__
//#include <asm/pgtable-3level.h>
#include <asm/processor.h>

extern pgd_t swapper_pg_dir[1024];
extern void paging_init(void);

extern unsigned long empty_zero_page[1024];

#define _PAGE_PRESENT	0x001
#define _PAGE_RW	0x002
#define _PAGE_USER	0x004
#define _PAGE_PWT	0x008
#define _PAGE_PCD	0x010
#define _PAGE_ACCESSED	0x020
#define _PAGE_DIRTY	0x040
#define _PAGE_PSE	0x080	/* 4 MB (or 2MB) page, Pentium+, if present.. */
#define _PAGE_GLOBAL	0x100	/* Global TLB entry PPro+ */





#define __pgprot(x) ((pgprot_t){ (x) })

#define PAGE_SHARED __pgprot(_PAGE_PRESENT|_PAGE_RW|_PAGE_USER|_PAGE_ACCESSED)
#endif



#define MAKE_GLOBAL(x) __pgprot((x)|_PAGE_GLOBAL)


#define _KERNPG_TABLE (_PAGE_PRESENT|_PAGE_RW|_PAGE_ACCESSED|_PAGE_DIRTY)

#define __PAGE_KERNEL (_PAGE_PRESENT|_PAGE_RW|_PAGE_DIRTY|_PAGE_ACCESSED)

#define PAGE_KERNEL MAKE_GLOBAL(__PAGE_KERNEL)

#define _KERNEL_TABLE (_PAGE_PRESENT|_PAGE_RW|_PAGE_ACCESSED|_PAGE_DIRTY)

#define TWOLEVEL_PGDIR_SHIFT 22
#define BOOT_USER_PGD_PTRS (__PAGE_OFFSET >> TWOLEVEL_PGDIR_SHIFT)
#define BOOT_KERNEL_PGD_PTRS (1024 - BOOT_USER_PGD_PTRS)

#define page_to_mb(x) ((x) >> (20 - PAGE_SHIFT))


#define pmd_page(pmd) ((unsigned long)__va(pmd_val(pmd) &PAGE_MASK))


#define pgd_index(address) ((address >> PGDIR_SHIFT)&(PTRS_PER_PGD-1))
#define __pgd_offset(address) pgd_index(address)

#define __pmd_offset(address) (((address) >> PMD_SHIFT)&(PTRS_PER_PMD -1))


#define __pte_offset(address) ((address >> PAGE_SHIFT)&(PTRS_PER_PTE-1))
#define pte_offset(dir,address) ((pte_t*)pmd_page(*(dir)) + __pte_offset(address))
#define __beep() asm("movb $0x3,%al;outb %al,$0x61")

#define PGDIR_SIZE (1UL << PGDIR_SHIFT)

#define USER_PTRS_PER_PGD (TASK_SIZE/PGDIR_SIZE)



#define PMD_SIZE (1UL << PMD_SHIFT)
#define PMD_MASK (~(PMD_SIZE - 1))


#define mk_pte_phys(physpage,pgprot) __mk_pte((physpage) >> PAGE_SHIFT,pgprot)


#define pmd_none(x) (!pmd_val(x))

#define __flush_tlb() \
	do{	\
		unsigned int tmpreg;\
		__asm__ __volatile__(\
			"movl %%cr3,%0;"\
			"movl %0,%%cr3;\n"\
			:"=r"(tmpreg)\
			::"memory");\
	}while(0)

#define __flush_tlb_all() \
	do{\
		__flush_tlb();\
	}while(0)

#define pgd_offset(mm,address) ((mm)->pgd + pgd_index(address))
#define pgd_offset_k(address) pgd_offset(&init_mm,address)

#endif
