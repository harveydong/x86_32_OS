#include <linux/init.h>
#include <asm/page.h>
#include <linux/bootmem.h>

static unsigned char *trampoline_base;


void __init smp_alloc_memory(void)
{
	trampoline_base = (void*)alloc_bootmem_low_pages(PAGE_SIZE);
	
	if(__pa(trampoline_base) >= 0x9F000)
		BUG();
}
void __init initialize_secondary(void)
{


}
