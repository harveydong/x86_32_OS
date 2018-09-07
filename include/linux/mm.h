#ifndef __MM_H_
#define __MM_H_
#include <linux/sched.h>
#include <asm/page.h>




#define VM_READ		0x00000001	/* currently active flags */
#define VM_WRITE	0x00000002
#define VM_EXEC		0x00000004
#define VM_SHARED	0x00000008

struct vm_area_struct{
	struct mm_struct *vm_mm;
	unsigned long vm_start;
	unsigned long vm_end;
	struct vm_area_struct *vm_next;
	pgprot_t vm_page_prot;
	unsigned long vm_flags;
	short vm_avl_height;
	struct vm_area_struct *vm_avl_left;
	struct vm_area_struct *vm_avl_right;	

};

typedef struct page{
	unsigned long index;
}mem_map_t;
#endif
