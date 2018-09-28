#include <linux/init.h>
#include <asm/page.h>
#include <linux/bootmem.h>
#include <linux/threads.h>
#include <asm/processor.h>

static unsigned char *trampoline_base;

int smp_num_cpus = 1;

struct cpuinfo_x86 cpu_data[NR_CPUS];

void __init smp_alloc_memory(void)
{
	trampoline_base = (void*)alloc_bootmem_low_pages(PAGE_SIZE);
	
	if(__pa(trampoline_base) >= 0x9F000)
	{//		BUG();
		printk("smp alloc memory base:0x%x\n",__pa(trampoline_base));	
		while(1);
	}
	printk("ok here smp alloc memory base:0x%x\n",__pa(trampoline_base));	
}
void __init initialize_secondary(void)
{


}
