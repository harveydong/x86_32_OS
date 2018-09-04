#ifndef __DESC_H_
#define __DESC_H_

#define __FIRST_TSS_ENTRY 12
#define __TSS(n) (((n) << 2)+__FIRST_TSS_ENTRY)


#ifndef __ASSEMBLY__
struct desc_struct{
	unsigned long a,b;
};

extern struct desc_struct gdt_table[];
extern struct desc_struct *idt,*gdt;

#endif
#endif
