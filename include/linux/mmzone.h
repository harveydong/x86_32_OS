#ifndef __MMZONE_H_
#define __MMZONE_H_
#include <linux/bootmem.h>
#include <linux/mm.h>

struct bootmem_data;

typedef struct zone_struct{
	char *name;

}zone_t;


typedef struct zonelist_struct{

	int gfp_mask;

}zonelist_t;

#define ZONE_DMA 0
#define ZONE_NORMAL 1
#define ZONE_HIGHMEM 2
#define MAX_NR_ZONES 3


#define NR_GFPINDEX 0x100

typedef struct pglist_data{
	zone_t node_zones[MAX_NR_ZONES];
	zonelist_t node_zonelists[NR_GFPINDEX];
	struct page *node_mem_map;
	unsigned long *valid_addr_bitmap;
	struct bootmem_data *bdata;

	struct pglist_data *node_next;

}pg_data_t;



extern pg_data_t contig_page_data;

extern pg_data_t *pgdat_list;
#endif
