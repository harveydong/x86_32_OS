#include <asm/processor-flags.h>
#include <asm/system.h>

#include <asm/processor.h>
#include <asm/pgtable.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/init.h>
#include <linux/printk.h>

#include <asm/e820.h>
#include <asm/boot.h>
#include <linux/ioport.h>
#include <asm/io.h>
#include <linux/sched.h>
#include <asm/highmem.h>
#include <linux/bootmem.h>
#include <asm/smp.h>
#include <asm/mpspec.h>
#include <asm/apic.h>
#include <asm/mmu_context.h>
#include <asm/desc.h>
#include <asm/atomic.h>
#include <asm/system.h>

unsigned long mmu_cr4_features = X86_CR4_PAE;

extern char _text,_etext,_edata,_end;

unsigned long cpu_initialized = 0;
int cpus_initialized = 0;
__u16 boot_cpu_addr;




#define PARAM ((unsigned char*)empty_zero_page)

static struct boot_params *setup_params =(struct boot_params*) (empty_zero_page);


#define ORIG_ROOT_DEV (*(unsigned short*)(PARAM+0x1FC))

#define EXT_MEM_K (*(unsigned short *)(PARAM + 2))
#define ALT_MEM_K (*(unsigned short *)(PARAM + 0x1e0))







#define E820_MAP_NR (int)(setup_params->e820_entries)

#define E820_MAP ((struct e820entry *)(setup_params->e820))



struct resource standard_io_resources[] = {
	[0] = {.name = "dma1",.start = 0x00, .end = 0x1f, .flags = IORESOURCE_BUSY},
	[1] = {.name = "pic1", .start =  0x20, .end = 0x3f, .flags = IORESOURCE_BUSY},
	[2] = {.name = "timer", .start = 0x40, .end = 0x5f, .flags = IORESOURCE_BUSY},
	[3] = {.name = "keyboard", .start = 0x60, .end = 0x6f, .flags = IORESOURCE_BUSY},
	[4] = {.name = "dma page reg", .start = 0x80, .end = 0x8f, .flags = IORESOURCE_BUSY}
};
#define STANDARD_IO_RESOURCES (sizeof(standard_io_resources) /sizeof(struct resource))


static struct resource code_resource = {
	.name = "Kernel Code",
	.start = 0x100000,
	.end = 0	

};

static struct resource data_resource = {
	.name = "Kernel data",
	.start = 0,
	.end = 0
};

static struct resource vram_resource ={

	.name = "Video RAM area",
	.start = 0xa0000,
	.end = 0xbffff,
	.flags = IORESOURCE_BUSY
};

struct cpuinfo_x86 boot_cpu_data = {0,0,0,0,-1,1,0,0,-1};

struct e820map e820 = {0};


static inline void load_LDT(struct mm_struct *mm)
{
	int cpu = smp_processor_id();
	void *segments = mm->context.segments;
	int count = LDT_ENTRIES;
	
	if(!segments){
		segments = &default_ldt[0];
		count = 5;
	}

	set_ldt_desc(cpu,segments,count);
	__load_LDT(cpu);
}

void __init cpu_init(void)
{
	int nr = smp_processor_id();

	struct tss_struct *t = &init_tss[nr];
	
	if(test_and_set_bit(nr,&cpu_initialized)){
		printk("CPU #%d already initialized\n",nr);
		for(;;)__sti();
	}
	printk("Initializing CPU#%d\n",nr);	


	__asm__ __volatile__("lgdt %0":"=m"(gdt_descr));
	__asm__ __volatile__("lidt %0":"=m"(idt_descr));
	
	__asm__("pushfl;andl $0xffffbfff,(%esp);popfl");
	
	atomic_inc(&init_mm.mm_count);
	current->active_mm = &init_mm;
	
	if(current->mm)
		BUG();

	enter_lazy_tlb(&init_mm,current,nr);

	t->esp0 = current->thread.esp0;
	set_tss_desc(nr,t);
		
	gdt_table[__TSS(nr)].b &= 0xFFFFFDFF;
	load_TR(nr);
	load_LDT(&init_mm);

#define CD(reg) __asm__("movl %0,%%db"#reg::"r"(0));
	CD(0);CD(1);CD(2);CD(3);	
#undef CD

	current->flags &= ~PF_USEDFPU;
	current->used_math = 0;
	stts();

}

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

#define MAXROMS 6
static struct resource rom_resources[MAXROMS] = {
	[0] = {
		.name = "System ROM",
		.start = 0xF0000,
		.end = 0xFFFFF,
		.flags = IORESOURCE_BUSY
	},
	[1] = {

		.name = "Video ROM",	
		.start = 0xc0000,
		.end = 0xc7fff,
		.flags = IORESOURCE_BUSY
	}

};

#define romsignature(x) (*(unsigned short*)(x) == 0xaa55)

static void __init probe_roms(void)
{
	int roms = 1;
	unsigned long base;
	unsigned char *romstart;

	request_resource(&iomem_resource,rom_resources+0);


	for(base = 0xC0000; base < 0xE0000; base += 2048){
		romstart = bus_to_virt(base);
		if(!romsignature(romstart))
			continue;
		
		request_resource(&iomem_resource,rom_resources + roms);
		roms++;
		break;
	}

	
}


void __init setup_arch(char **cmdline_p)
{

	unsigned long bootmap_size;
	unsigned long start_pfn,max_pfn,max_low_pfn;
	int i;




	ROOT_DEV = to_kdev_t(ORIG_ROOT_DEV);	


	setup_memory_region();

	init_mm.start_code = (unsigned long)&_text;
	init_mm.end_code = (unsigned long)&_etext;
	init_mm.end_data = (unsigned long)&_edata;
	init_mm.brk = (unsigned long)&_end;
	
	code_resource.start = virt_to_bus(&_text);
	code_resource.end = virt_to_bus(&_etext) - 1;
	data_resource.start = virt_to_bus(&_etext);
	data_resource.end = virt_to_bus(&_edata) - 1;

#define PFN_UP(x) (((x) + PAGE_SIZE - 1) >> PAGE_SHIFT)
#define PFN_DOWN(x) ((x) >> PAGE_SHIFT)
#define PFN_PHYS(x) ((x) << PAGE_SHIFT)


#define VMALLOC_RESERVE (unsigned long)(128 << 20)
#define MAXMEM (unsigned long)(-PAGE_OFFSET - VMALLOC_RESERVE)
#define MAXMEM_PFN PFN_DOWN(MAXMEM)
#define MAX_NONPAE_PFN (1<<20)


	start_pfn = PFN_UP(__pa(&_end));	
	max_pfn = 0;
	
	for(i = 0; i < e820.nr_map; i++){
		unsigned long start,end;
		
		if(e820.map[i].type !=  E820_RAM)
			continue;
	
		start = PFN_UP(e820.map[i].addr);
		end = PFN_DOWN(e820.map[i].addr + e820.map[i].size);

		if(start >= end)
			continue;
		if(end > max_pfn)
			max_pfn = end;
	}

	printk("max_pfn is %lx\n",max_pfn);

	max_low_pfn = max_pfn;
	
	if(max_low_pfn > MAXMEM_PFN){
		max_low_pfn = MAXMEM_PFN;
	
	}
	
	highstart_pfn = highend_pfn = max_pfn;
	if(max_pfn > MAXMEM_PFN){
		highstart_pfn = MAXMEM_PFN;
		printk("%ldMB HIGHMEM available.\n",page_to_mb(highend_pfn - highstart_pfn));
	}

	bootmap_size = init_bootmem(start_pfn,max_low_pfn);
	printk("max low pfn:0x%x\n",max_low_pfn);
	for(i = 0; i < e820.nr_map;i++){
		unsigned long curr_pfn,last_pfn,size;
	
		if(e820.map[i].type != E820_RAM)
			continue;

		curr_pfn = PFN_UP(e820.map[i].addr);
		if(curr_pfn >= max_low_pfn)
			continue;

		last_pfn = PFN_DOWN(e820.map[i].addr + e820.map[i].size);
		if(last_pfn > max_low_pfn)
			last_pfn = max_low_pfn;
		
		if(last_pfn <= curr_pfn)
			continue;

		size = last_pfn - curr_pfn;
		free_bootmem(PFN_PHYS(curr_pfn),PFN_PHYS(size));
	}
	reserve_bootmem(HIGH_MEMORY,(PFN_PHYS(start_pfn)+bootmap_size + PAGE_SIZE -1) - (HIGH_MEMORY));
	reserve_bootmem(0,PAGE_SIZE);
	reserve_bootmem(PAGE_SIZE,PAGE_SIZE);
	smp_alloc_memory();

	find_smp_config();

	paging_init();
//	printk("virt to phys:%0x\n",virt_to_phys(0xc0002000));

	if(smp_found_config)
		get_smp_config();


	init_apic_mapping();

	probe_roms();


	for(i = 0; i < e820.nr_map;i++){

		struct resource *res;
		if(e820.map[i].addr + e820.map[i].size > 0x100000000ULL)
			continue;
		
		res = alloc_bootmem_low(sizeof(struct resource));
		switch(e820.map[i].type){
		case E820_RAM:
			res->name = "System RAM";break;
		case E820_ACPI: 
			res->name = "ACPI Tables";break;
		case E820_NVS: 
			res->name = "ACPI Non-volatile Storage";break;

		default:
			res->name = "Reserved";
		
		}

		res->start = e820.map[i].addr;
		res->end = res->start + e820.map[i].size - 1;
		request_resource(&iomem_resource,res);
		if(e820.map[i].type == E820_RAM){
			request_resource(res,&code_resource);
			request_resource(res,&data_resource);
		}
	}

	request_resource(&iomem_resource,&vram_resource);

	for(i = 0; i < STANDARD_IO_RESOURCES;i++){
		request_resource(&ioport_resource,standard_io_resources + i);
	}
}
