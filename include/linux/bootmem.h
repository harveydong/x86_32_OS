#ifndef __BOOTMEM_H_
#define __BOOTMEM_H_
#include <linux/init.h>
#include <linux/cache.h>
#include <asm/dma.h>
#include <linux/mmzone.h>

typedef struct bootmem_data{
	unsigned long node_boot_start;
	unsigned long node_low_pfn;
	void *node_bootmem_map;
	unsigned long last_offset;
	unsigned long last_pos;

}bootmem_data_t;

extern unsigned long max_low_pfn;
extern unsigned long min_low_pfn;


extern unsigned long __init init_bootmem (unsigned long addr, unsigned long memend);
void __init free_bootmem(unsigned long addr,unsigned long size);
void __init reserve_bootmem (unsigned long addr, unsigned long size);

extern void * __init __alloc_bootmem (unsigned long size, unsigned long align, unsigned long goal);

extern void*__init __alloc_bootmem_node(pg_data_t *pgdat,unsigned long size,unsigned long align,unsigned long goal);


#define alloc_bootmem_low_pages(x) \
	__alloc_bootmem((x),PAGE_SIZE,0)


#define alloc_bootmem_node(pgdat,x)\
	__alloc_bootmem_node((pgdat),(x),SMP_CACHE_BYTES,__pa(MAX_DMA_ADDRESS))
#endif
