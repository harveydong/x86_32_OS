#ifndef __PAGE_64_TYPES_H_
#define __PAGE_64_TYPES_H_


#define PGDIR_SHIFT 39
#define PTRS_PER_PGD 512

#define PUD_SHIFT 30
#define PTRS_PER_PUD 512
	

#define PMD_SHIFT 21
#define PTRS_PER_PMD 512

#define PTRS_PER_PTE 512

#define PMD_SIZE (1 << PMD_SHIFT)
#define PMD_MASK (~(PMD_SIZE - 1))


#define __START_KERNEL_map 0xffffffff80000000
#define __PAGE_OFFSET 0xffff880000000000

#define PAGE_OFFSET ((unsigned long)__PAGE_OFFSET)

#define KERNEL_IMAGE_SIZE (512*1024*1024)
#define PMD_PAGE_SIZE (1 << PMD_SHIFT)
#define PMD_PAGE_MASK (~(PMD_PAGE_SIZE -1 ))


#define PAGE_SHIFT 12
#define PAGE_SIZE (1 << PAGE_SHIFT)

#define THREAD_ORDER 1
#define THREAD_SIZE (PAGE_SIZE << THREAD_ORDER)

#define IRQ_STACK_ORDER 2
#define IRQ_STACK_SIZE (PAGE_SIZE << IRQ_STACK_ORDER)
#define __phys_reloc_hide(x) (x)

#endif
