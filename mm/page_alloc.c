#include <linux/mmzone.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/list.h>
#include <linux/string.h>
#include <linux/bootmem.h>
#include <linux/swapctl.h>

struct list_head active_list;
struct list_head inactive_dirty_list;

pg_data_t *pgdat_list;


static char *zone_names[MAX_NR_ZONES] = {"DMA","Normal","HighMem"};
static int zone_balance_ratio[MAX_NR_ZONES] = {32,128,128};
static int zone_balance_min[MAX_NR_ZONES] ={10,10,10};
static int zone_balance_max[MAX_NR_ZONES] = {255,255,255};


#define memlist_init(x) INIT_LIST_HEAD(x)

#define LONG_ALIGN(x) (((x) + (sizeof(long)) -1)&~((sizeof(long)) - 1))


static inline void build_zonelists(pg_data_t*pgdat)
{

	int i,j,k;
		
	for(i = 0;i < NR_GFPINDEX;i++){
		zonelist_t *zonelist;
		zone_t *zone;
		zonelist = pgdat->node_zonelists + i;
		memset(zonelist,0,sizeof(*zonelist));
		
		zonelist->gfp_mask = i;
		j = 0;
		k = ZONE_NORMAL;
	
		if(i & __GFP_HIGHMEM)
			k = ZONE_HIGHMEM;
		if(i & __GFP_DMA)
			k = ZONE_DMA;
		
		switch(k){
			default:
				BUG();

			case ZONE_HIGHMEM:
				zone = pgdat->node_zones + ZONE_HIGHMEM;
				
				if(zone->size){
					zonelist->zones[j++] = zone;
				}
			case ZONE_NORMAL:
				zone = pgdat->node_zones + ZONE_NORMAL;
				if(zone->size)
					zonelist->zones[j++] = zone;
			
			case ZONE_DMA:
				zone = pgdat->node_zones + ZONE_DMA;
				if(zone->size)
					zonelist->zones[j++] = zone;
		}	
		
		zonelist->zones[j++] = NULL;
	}

}

void __init free_area_init_core(int nid,pg_data_t *pgdat,struct page**gmap,unsigned long *zones_size,unsigned long zone_start_paddr,unsigned long *zholes_size,struct page*lmem_map)
{
	struct page *p;
	unsigned long totalpages,offset,realtotalpages;
	unsigned long i,j;
	unsigned long map_size;
	unsigned int cumulative = 0;

	totalpages = 0;
	
	for(i = 0; i < MAX_NR_ZONES;i++)
	{
		unsigned long size = zones_size[i];
		totalpages += size;
	}	

	realtotalpages = totalpages;
	
	if(zholes_size)
		for(i = 0; i < MAX_NR_ZONES;i++)
			realtotalpages -= zholes_size[i];

	printk("On node %d totalpages:%lu\n",nid,realtotalpages);

	memlist_init(&active_list);
	memlist_init(&inactive_dirty_list);
	
	map_size = (totalpages+1)*sizeof(struct page);


	if(lmem_map == (struct page*)0){
	
		printk("map_size:%d\n",map_size);
 
		lmem_map = (struct page*)alloc_bootmem_node(pgdat,map_size);
	}	
 
#if 0
		lmem_map = (struct page*)(PAGE_OFFSET + MAP_ALIGN((unsigned long )lmem_map - PAGE_OFFSET));
	}



 
	*gmap = pgdat->node_mem_map = lmem_map;
	pgdat->node_size = totalpages;
	pgdat->node_start_paddr = zone_start_paddr;
	pgdat->node_start_mapnr = (lmem_map - mem_map);

	
	for(p = lmem_map;p < lmem_map+totalpages;p++){
		set_page_count(p,0);
		SetPageReserved(p);
		init_waitqueue_head(&p->wait);
		memlist_init(&p->list);
	}

	offset = lmem_map - mem_map;
	for(j = 0;j < MAX_NR_ZONES;j++){
		zone_t *zone = pgdat->node_zones +j;
		unsigned long mask;
		unsigned long size,realsize;
	
		realsize = size = zones_size[j];
		if(zholes_size)
			realsize -= zholes_size[j];

		printk("zone(%lu):%lu pages.\n",j,size);
		zone->size = size;
		zone->name = zone_names[j];
		zone->lock = SPIN_LOCK_UNLOCKED;
		zone->zone_pgdat = pgdat;
		zone->free_pages = 0;
		zone->inactive_clean_pages = 0;	
		zone->inactive_dirty_pages = 0;
		memlist_init(&zone->inactive_clean_list);
		
		if(!size)
			continue;

		zone->offset = offset;
		cumulative += size;
		mask = (realsize / zone_balance_ratio[j]);
		if(mask < zone_balance_min[j])
			mask = zone_balance_min[j];
		else if(mask > zone_balance_max[j])
			mask = zone_balance_max[j];
	
		zone->pages_min = mask;
		zone->pages_low = mask*2;
		zone->pages_high = mask*3;

		freepages.min += mask;
		freepages.low += mask*2;
		freepages.high += mask*3;
		
		zone->zone_mem_map = mem_map + offset;
		zone->zone_start_mapnr = offset;
		zone->zone_start_paddr = zone_start_paddr;
	
		for(i = 0; i < size;i++){
			struct page *page = mem_map + offset + i;
			page->zone = zone;
			if(j != ZONE_HIGHMEM){
				page->virtual = __va(zone_start_paddr);
				zone_start_paddr += PAGE_SIZE;
			}
		}
			
		offset += size;
		mask = -1;
	
		for(i = 0; i < MAX_ORDER;i++){
			unsigned long bitmap_size;
			memlist_init(&zone->free_area[i].free_list);
			
			mask += mask;
			size = (size + ~mask) &mask;
			bitmap_size = size >> i;
			bitmap_size = (bitmap_size + 7) >> 3;
			bitmap_size = LONG_ALIGN(bitmap_size);	
			zone->free_area[i].map = (unsigned int*)alloc_bootmem_node(pgdat,bitmap_size);		
		}
	}

	build_zonelists(pgdat);
#endif
}
void __init free_area_init(unsigned long *zones_size)
{
	free_area_init_core(0,&contig_page_data,&mem_map,zones_size,0,0,0);
}
