#ifndef __PGTABLE_H_
#define __PGTABLE_H_


#include <asm/page.h>

#ifndef __ASSEMBLY__
extern pgd_t swapper_pg_dir[1024];

#define _PAGE_PRESENT	0x001
#define _PAGE_RW	0x002
#define _PAGE_USER	0x004
#define _PAGE_PWT	0x008
#define _PAGE_PCD	0x010
#define _PAGE_ACCESSED	0x020
#define _PAGE_DIRTY	0x040
#define _PAGE_PSE	0x080	/* 4 MB (or 2MB) page, Pentium+, if present.. */
#define _PAGE_GLOBAL	0x100	/* Global TLB entry PPro+ */



typedef struct {
	unsigned long pgprot;
}pgprot_t;

#define __pgprot(x) ((pgprot_t){ (x) })

#define PAGE_SHARED __pgprot(_PAGE_PRESENT|_PAGE_RW|_PAGE_USER|_PAGE_ACCESSED)
#endif


#define TWOLEVEL_PGDIR_SHIFT 22
#define BOOT_USER_PGD_PTRS (__PAGE_OFFSET >> TWOLEVEL_PGDIR_SHIFT)
#define BOOT_KERNEL_PGD_PTRS (1024 - BOOT_USER_PGD_PTRS)


#endif
