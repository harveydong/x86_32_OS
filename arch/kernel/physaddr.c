#include "page_64_types.h"
#include "physaddr.h"

unsigned long __phys_addr(unsigned long x)
{
	if(x >= __START_KERNEL_map){
		x -= __START_KERNEL_map;
		x += phys_base;
	}else{
		x -= PAGE_OFFSET;
	}

	return x;
}
