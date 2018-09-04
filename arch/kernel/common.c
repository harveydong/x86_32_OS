#include "percpu.h"
#include "processor.h"
#include "desc.h"
#include "../../../segment.h"
#include "desc_defs.h"
#include "irq.h"
#include "cache.h"
#include "sched.h"

DEFINE_PER_CPU_PAGE_ALIGNED(struct gdt_page,gdt_page) = {
	.gdt = {
		[GDT_ENTRY_KERNEL32_CS] = GDT_ENTRY_INIT(0xc09b,0,0xfffff),
		[GDT_ENTRY_KERNEL_CS] = GDT_ENTRY_INIT(0xa09b,0,0xfffff),
		[GDT_ENTRY_KERNEL_DS] = GDT_ENTRY_INIT(0xc093,0,0xfffff),
		[GDT_ENTRY_DEFAULT_USER32_CS] = GDT_ENTRY_INIT(0xc0fb,0,0xfffff),
		[GDT_ENTRY_DEFAULT_USER_DS] = GDT_ENTRY_INIT(0xc0f3,0,0xfffff),	
		[GDT_ENTRY_DEFAULT_USER_CS] = GDT_ENTRY_INIT(0xa0fb,0,0xfffff),

	}
};
DECLARE_PER_CPU_FIRST(union irq_stack_union,irq_stack_union);
DECLARE_INIT_PER_CPU(irq_stack_union);

DEFINE_PER_CPU_FIRST(union irq_stack_union,irq_stack_union) __attribute__((aligned(PAGE_SIZE)));


DEFINE_PER_CPU(struct task_struct *,current_task) ____cacheline_aligned = &init_task;

struct desc_ptr idt_descr = {NR_VECTORS*16 -1,(unsigned long)idt_table};
