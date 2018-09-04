#ifndef __IRQ_FUNC_H_
#define __IRQ_FUNC_H_
#include "irq.h"



static inline void pack_gate(gate_desc *gate,unsigned type,unsigned long func,unsigned dpl,unsigned ist,unsigned seg)
{
	gate->offset_low = PTR_LOW(func);
	gate->segment = __KERNEL_CS;
	gate->ist = ist;	
	gate->p = 1;
	gate->dpl = dpl;
	gate->zero0 = 0;
	gate->zero1 = 0;
	gate->type = type;
	gate->offset_middle = PTR_MIDDLE(func);
	gate->offset_high = PTR_HIGH(func);
}
static inline void native_write_idt_entry(gate_desc *idt,int entry,const gate_desc *gate)
{
	memcpy(&idt[entry],gate,sizeof(*gate));
}

#define write_idt_entry(dt,entry,g) native_write_idt_entry(dt,entry,g)


static inline void _set_gate(int gate,unsigned type,void *addr,unsigned dpl,unsigned ist,unsigned seg)
{
	gate_desc s;
	pack_gate(&s,type,(unsigned long)addr,dpl,ist,seg);
	
	write_idt_entry(idt_table,gate,&s);

}

static inline void set_intr_gate(unsigned int n,void *addr)
{
	_set_gate(n,GATE_INTERRUPT,addr,0,0,__KERNEL_CS);
}

static inline void native_load_idt(const struct desc_ptr *dtr)
{
	asm volatile("lidt %0"::"m"(*dtr));
}
#define load_idt(dtr) native_load_idt(dtr)
#endif
