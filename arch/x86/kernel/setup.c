#include <asm/processor-flags.h>
#include <asm/processor.h>
#include <asm/pgtable.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/init.h>
#include <linux/printk.h>

#include <asm/e820.h>
#include <asm/boot.h>


unsigned long mmu_cr4_features = X86_CR4_PAE;

#define PARAM ((unsigned char*)empty_zero_page)

static struct boot_params *setup_params =(struct boot_params*) (empty_zero_page);


#define ORIG_ROOT_DEV (*(unsigned short*)(PARAM+0x1FC))

#define EXT_MEM_K (*(unsigned short *)(PARAM + 2))
#define ALT_MEM_K (*(unsigned short *)(PARAM + 0x1e0))

#define E820_MAP_NR (int)(setup_params->e820_entries)

#define E820_MAP ((struct e820entry *)(setup_params->e820))



struct cpuinfo_x86 boot_cpu_data = {0,0,0,0,-1,1,0,0,-1};

struct e820map e820 = {0};



void __init add_memory_region(u32 start,u32 size,u32 type)
{
	int x = e820.nr_map;

	if(x >= E820MAX){
		printk("Ooops! to many entries in the memory map!\n");
		return;
	}

	e820.map[x].addr = start;
	e820.map[x].size = size;
	e820.map[x].type = type;
	e820.nr_map++;
	
	//printk("x:%d,start:%x,size:%x,type:%x\n",x,start,size,type);
	//printk("x:%d, e820 start:%x,size:%x,type:%x\n",x,(u32)e820.map[x].addr,(u32)e820.map[x].size,(u32)e820.map[x].type);
}
static int __init copy_e820_map(struct e820entry *biosmap,int nr_map)
{
	int nr = nr_map;

	//printk("setup params and %x,%x,%x,%x,%x\n",setup_params[0],setup_params[1],setup_params[2],setup_params[3],setup_params[4],setup_params[5]);

/*
	printk("setup params:%x\n",setup_params[2]);
	printk("setup params:%x\n",setup_params[3]);
	printk("setup params:%x\n",setup_params[4]);
	printk("setup params:%x\n",setup_params[5]);
	printk("setup params:%x\n",setup_params[6]);
*/
//	printk("copy e820 map:nr_map is %d,entries:%d\n",nr_map,setup_params->e820_entries);
	if(nr_map < 2)
		return -1;

	do{
		u32 start = biosmap->addr;
		u32 size =biosmap->size;
		u32 end = start + size;	
		u32 type = biosmap->type;

	//	printk("start:%lx, size:%lx,type:%x\n",start,size,type);
	//	printk("biosmap entries:%x\n",biosmap);
	//	printk("size addr:%x\n",&biosmap->size);
		if(start > end && (nr_map < nr)){
			end = 0xffffffff;
			size = end - start;
		}else if(start > end)
			return -1;

		if(type == E820_RAM){
			if(start < 0x100000ULL && end > 0xA0000ULL){
				if(start < 0xA0000ULL)
					add_memory_region(start,0xA0000ULL - start,type);
				if(end <= 0x100000ULL)
					continue;
				start = 0x100000ULL;
				size = end - start;	
			}
		}
		add_memory_region(start,size,type);
	}while(biosmap++,--nr_map);	
	
	return 0;
}


static void __init print_memory_map(char *who)
{
	int i;

	u32 size;
	u32 addr;

	for (i = 0; i < e820.nr_map; i++) {
		size = e820.map[i].size;
		addr = e820.map[i].addr;
		printk(" %s: %016Lx @ %016Lx ", who,size,addr);

		switch (e820.map[i].type) {
		case E820_RAM:	printk("(usable)\n");
				break;
		case E820_RESERVED:
				printk("(reserved)\n");
				break;
		case E820_ACPI:
				printk("(ACPI data)\n");
				break;
		case E820_NVS:
				printk("(ACPI NVS)\n");
				break;
		default:	printk("type %lu\n", e820.map[i].type);
				break;
		}
	}
}


#define LOWMEMSIZE() (0x9f000)

void __init setup_memory_region(void)
{
	char *who = "BIOS-e820";
	
	if(copy_e820_map(E820_MAP,E820_MAP_NR) < 0){
		unsigned long mem_size;
		
		if(ALT_MEM_K < EXT_MEM_K){
			mem_size = EXT_MEM_K;
			who = "BIOS-88";
		}else{
			mem_size = ALT_MEM_K;
			who = "BIOS-e801";	
		}

		e820.nr_map = 0;
		add_memory_region(0,LOWMEMSIZE(),E820_RAM);
		add_memory_region(HIGH_MEMORY,(mem_size << 10) - HIGH_MEMORY,E820_RAM);
	}

	printk("BIOS-provied physical RAM map:\n");
	print_memory_map(who);
}
void __init setup_arch(char **cmdline_p)
{
	ROOT_DEV = to_kdev_t(ORIG_ROOT_DEV);	
	
	setup_memory_region();
}
