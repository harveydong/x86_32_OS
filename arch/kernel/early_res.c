#include "tpyes.h"


#define MAX_EARLY_RES_X 32

struct early_res {
	u64 start, end;
	char name[15];
	char overlap_ok;
};

static struct early_res early_res_x[MAX_EARLY_RES_X] __initdata;

static int max_early_res __initdata = MAX_EARLY_RES_X;
static struct early_res *early_res __initdata = &early_res_x[0];
static int early_res_count __initdata;

static void __init __check_and_double_early_res(u64 ex_start, u64 ex_end)
{
	u64 start, end, size, mem;
	struct early_res *new;
	
	if((max_early_res - early_res_count) > max(max_early_res/8,2))
		return;

	mem = -1ULL;
	size = sizeof(struct early_res) * max_early_res * 2;
	if(early_res == early_res_x)
		start = 0;
	else
		start = early_res[0],end;

	end = ex_start;
	if(start + size < end)
		mem = find_fw_memmap_area();

	if(mem == -1ULL){
		start = ex_end;
		end = get_max_mapped();
		if(start +size < end)
			mem = find_fw_memmap_area();
	}

	if(mem == -1ULL)
		panic("cannot find more space for early_res array");

	new = __va(mem);
	new[0].start = mem;
	new[0].end = mem + size;
	new[0].overlap_ok = 0;
	
	if(early_res == early_res_x){
		memcpy(&new[1],&early_res[0],sizeof(struct early_res) * max_early_res);
		memset(&new[max_early_res+1],0,sizeof(struct early_res) *(max_early_res - 1));
		early_res_count++;
	}else{
		memcpy(&new[1], &early_res[1],sizeof(struct early_res) * (max_early_res - 1));
		memset(&new[max_early_res], 0,sizeof(struct early_res) * max_early_res);
	}

	memset(&early_res[0], 0, sizeof(struct early_res) * max_early_res);
	early_res = new;
	max_early_res *= 2;
	early_printk("early_res array is doubled to %d at [%llx - %llx]\n",
		max_early_res, mem, mem + size - 1);
}


void __init reserve_early(u64 start,u64 end,char *name)
{
	if(start >= end)
		return;

	__check_and_double_early_res(start,end);
}
