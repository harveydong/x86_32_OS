#include "../../../../boot.h"


struct e820map e820;

u64 __init find_e820_area(u64 start, u64 end, u64 size, u64 align)
{
	int i;
	
	for(i = 0; i < e820.nr_map;i++){
		
	}

}

u64 __init find_fw_memmap_area(u64 start, u64 end, u64 size, u64 align)
{
	return find_e820_area(start, end, size, align);
}

