#ifndef __DESC_H_
#define __DESC_H_
#include  <asm/ldt.h>
 

#define __FIRST_TSS_ENTRY 12
#define __FIRST_LDT_ENTRY (__FIRST_TSS_ENTRY+1)

#define __TSS(n) (((n) << 2)+__FIRST_TSS_ENTRY)
#define __LDT(n) (((n) << 2) + __FIRST_LDT_ENTRY)

#ifndef __ASSEMBLY__
struct desc_struct{
	unsigned long a,b;
};

extern struct desc_struct gdt_table[];
extern struct desc_struct *idt,*gdt;

struct xgt_desc_struct{
	unsigned short size;
	unsigned int address __attribute__((packed));
};

#define idt_descr (*(struct xgt_desc_struct*)((char*)&idt - 2))
#define gdt_descr (*(struct xgt_desc_struct*)((char*)&gdt - 2))

#define load_TR(n) __asm__ __volatile__("ltr %%ax"::"a"(__TSS(n)<<3))
#define __load_LDT(n) __asm__ __volatile__("lldt %%ax"::"a"(__LDT(n)<<3))


extern struct desc_struct default_ldt[];
extern void set_tss_desc(unsigned int n,void *addr);

extern void set_ldt_desc(unsigned int n,void *addr,unsigned int size);

struct mm_struct;


#endif
#endif
