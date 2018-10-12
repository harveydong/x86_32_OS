#ifndef __MM_H_
#define __MM_H_
#include <linux/sched.h>
#include <asm/page.h>
#include <asm/atomic.h>
#include <linux/wait.h>
#include <asm/bitops.h>

#include <linux/list.h>
#include <linux/fs.h>


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


#define PG_locked 0
#define PG_referenced 1
#define PG_dirty 4
#define PG_decr_after 5
#define PG_active 6
#define PG_inactive_dirty 7
#define PG_swap_cache 9
#define PG_inactive_clean 11
#define PG_highmem	12
#define PG_reserved 31

typedef struct page{
	struct list_head list;
	unsigned long index;
	atomic_t count;
	unsigned long flags;
	wait_queue_head_t wait;
	void *virtual;
	struct zone_struct *zone;
	
	unsigned long age;

	struct buffer_head *buffers;
	struct address_space *mapping;

}mem_map_t;

#define set_page_count(p,v) atomic_set(&(p)->count,v)
#define SetPageReserved(page) set_bit(PG_reserved,&(page)->flags)
#define ClearPageReserved(page) clear_bit(PG_reserved,&(page)->flags)
#define PageReserved(page) test_bit(PG_reserved,&(page)->flags)
#define put_page_testzero(p) atomic_dec_and_test(&(p)->count)

#define set_page_count(p,v) atomic_set(&(p)->count,v)

#define PageSwapCache(page) test_bit(PG_swap_cache,&(page)->flags)
#define PageLocked(page) test_bit(PG_locked,&(page)->flags)
#define PageDecrAfter(page) test_bit(PG_decr_after,&(page)->flags)
#define PageActive(page) test_bit(PG_active,&(page)->flags)
#define PageInactiveDirty(page) test_bit(PG_inactive_dirty,&(page)->flags)
#define PageInactiveClean(page) test_bit(PG_inactive_clean,&(page)->flags)
extern mem_map_t *mem_map;


extern unsigned long max_mapnr;
extern unsigned long num_physpages;
extern void *high_memory;


extern void free_area_init(unsigned long *zones_size);

extern void mem_init(void);
#define __GFP_HIGH 0x02
#define __GFP_DMA 0x08
#define __GFP_HIGHMEM 0x10


extern void __free_pages(struct page*page,unsigned long order);

#define __free_page(page) __free_pages((page),0)

#define virt_to_page(kaddr) (mem_map + (__pa(kaddr) >> PAGE_SHIFT))

#define VALID_PAGE(page) ((page - mem_map) < max_mapnr)

#endif
