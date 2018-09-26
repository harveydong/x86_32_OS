#ifndef __MM_H_
#define __MM_H_
#include <linux/sched.h>
#include <asm/page.h>
#include <asm/atomic.h>
#include <linux/wait.h>
#include <asm/bitops.h>

#include <linux/list.h>


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

#define PG_reserved 31

typedef struct page{
	struct list_head list;
	unsigned long index;
	atomic_t count;
	unsigned long flags;
	wait_queue_head_t wait;
	void *virtual;
	struct zone_struct *zone;


}mem_map_t;

#define set_page_count(p,v) atomic_set(&(p)->count,v)
#define SetPageReserved(page) set_bit(PG_reserved,&(page)->flags)

extern mem_map_t *mem_map;

extern void free_area_init(unsigned long *zones_size);

#define __GFP_HIGH 0x02
#define __GFP_DMA 0x08
#define __GFP_HIGHMEM 0x10

#endif
