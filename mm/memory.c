#include <linux/mm.h>

mem_map_t *mem_map;
unsigned long max_mapnr;
unsigned long num_physpages;
struct page *highmem_start_page;

void *high_memory;
