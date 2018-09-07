#include <linux/init.h>
#include <linux/mmzone.h>
#include <asm/io.h>
#include <asm/page.h>
#include <linux/string.h>
#include <asm/bitops.h>


unsigned long max_low_pfn;
unsigned long min_low_pfn;

static unsigned long __init init_bootmem_core(pg_data_t *pgdat,unsigned long mapstart,unsigned long start,unsigned long end)
{
	bootmem_data_t *bdata = pgdat->bdata;
	unsigned long mapsize = ((end - start) + 7)/8;

	pgdat->node_next = pgdat_list;
	pgdat_list = pgdat;
	
	mapsize = (mapsize + (sizeof(long) - 1UL)) & ~(sizeof(long) - 1UL);
	bdata->node_bootmem_map = phys_to_virt(mapstart << PAGE_SHIFT);
	bdata->node_boot_start = (start << PAGE_SHIFT);
	bdata->node_low_pfn = end;

	memset(bdata->node_bootmem_map,0xff,mapsize);
	
	return mapsize;
}


unsigned long __init init_bootmem(unsigned long start,unsigned long pages)
{
	max_low_pfn = pages;
	min_low_pfn = start;
	
	return (init_bootmem_core(&contig_page_data,start,0,pages));
}


static void __init free_bootmem_core(bootmem_data_t *bdata,unsigned long addr,unsigned long size)
{
	unsigned long i;
	unsigned long start;

	unsigned long sidx;
	unsigned long eidx = (addr + size - bdata->node_boot_start)/PAGE_SIZE;
	unsigned long end = (addr + size)/PAGE_SIZE;

	if(end > bdata->node_low_pfn)
		BUG();
	start = (addr + PAGE_SIZE - 1)/PAGE_SIZE;
	sidx = start - (bdata->node_boot_start/PAGE_SIZE);
	
	for(i = sidx; i < eidx; i++){
		if(!test_and_clear_bit(i,bdata->node_bootmem_map))
			BUG();
	}
}
void __init free_bootmem(unsigned long addr,unsigned long size)
{
	return (free_bootmem_core(contig_page_data.bdata,addr,size));
}

static void __init reserve_bootmem_core(bootmem_data_t *bdata,unsigned long addr,unsigned long size)
{
	unsigned long i;
	unsigned long sidx = (addr - bdata->node_boot_start)/PAGE_SIZE;
	unsigned long eidx = (addr + size - bdata->node_boot_start + PAGE_SIZE - 1)/PAGE_SIZE;
	
	unsigned end = (addr + size + PAGE_SIZE - 1)/PAGE_SIZE;
	
	if(end > bdata->node_low_pfn)
		BUG();

	for(i = sidx; i < eidx; i++)
		if(test_and_set_bit(i,bdata->node_bootmem_map))
			printk("hm,page %081x reserved twice\n",i*PAGE_SIZE);
	

}

void __init reserve_bootmem(unsigned long addr,unsigned long size)
{
	reserve_bootmem_core(contig_page_data.bdata,addr,size);
}

static void *__init __alloc_bootmem_core(bootmem_data_t *bdata,unsigned long size,unsigned long align,unsigned long goal)
{

	int i;

	unsigned long start = 0;

	unsigned long areasize,preferred,incr;

	unsigned long eidx = bdata->node_low_pfn  - (bdata->node_boot_start >> PAGE_SHIFT);
	
	if(!size) BUG();

	if(goal && (goal >= bdata->node_boot_start) && ((goal >> PAGE_SHIFT)<bdata->node_low_pfn)){
		preferred = goal - bdata->node_boot_start;
	}else
		preferred = 0;

	preferred = ((preferred + align - 1)&~(align - 1)) >> PAGE_SHIFT;
	areasize = (size + PAGE_SIZE - 1)/PAGE_SIZE;
	incr = align >> PAGE_SHIFT ?:1;

restart_scan:

	for(i = preferred; i < eidx; i += incr){
		unsigned long j;
		if(test_bit(i,bdata->node_bootmem_map))
			continue;
		for(j = i +1; j < i + areasize; ++j){
			if(j >= eidx)
				goto fail_block;
			if(test_bit(j,bdata->node_bootmem_map))
				goto fail_block;
		}
		start = i;
		goto found;
fail_block:
		;
	}

	if(preferred){
		preferred = 0;
		goto restart_scan;
	}

found:
	if(start >= eidx)
		BUG();

}

void * __init __alloc_bootmem(unsigned long size,unsigned long align,unsigned long goal)
{
	pg_data_t *pgdat = pgdat_list;
	void *ptr;

	while(pgdat){
		if((ptr = __alloc_bootmem_core(pgdat->bdata,size,align,goal)))
			return (ptr);
		pgdat = pgdat->node_next;
	}

	BUG();
	return NULL;
}

unsigned long __init free_all_bootmem(void)
{

}
