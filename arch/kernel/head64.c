#include "init.h"
#include "sections.h"
#include "string.h"
#include "p_types.h"
#include "tlbflush.h"
#include "../segment.h"
#include "desc.h"
#include "irq_fun.h"
#include "kernel.h"
#include "desc_defs.h"
#include "page.h"
#include "../../../../boot.h"

struct boot_params boot_params __attribute__((aligned(16)));

static void __init clear_bss(void)
{
	memset(__bss_start,0,(unsigned long)__bss_stop - (unsigned long)__bss_start);

}

static void __init zap_identity_mappings(void)
{
	pgd_t *pgd = pgd_offset_k(0UL);
	pgd_clear(pgd);
	__flush_tlb_all();

}

static void __init copy_bootdata(char *real_mode_data)
{
	memcpy(&boot_params,real_mode_data,sizeof(boot_params));
}
void __init x86_64_start_reservations(char *real_mode_data)
{
	copy_bootdata(__va(real_mode_data));

	//reserve_early(__pa_symbol(&_text),__pa_symbol(&_bss_stop),"TEXT DATA BSS");

	start_kernel();
}
void __init x86_64_start_kernel(char *real_mode_data)
{

	int i;

	
	setup_early_printk();
	clear_bss();	

	zap_identity_mappings();

	//cleanup_highmap();

	for(i = 0; i < NUM_EXCEPTION_VECTORS;i++)
	{
		set_intr_gate(i,&early_idt_handlers[i]);

	}

	load_idt((const struct desc_ptr*)&idt_descr);
	
	early_printk("\n\n\nnihao ,I am here\n");
	
	x86_64_start_reservations(real_mode_data);
}
