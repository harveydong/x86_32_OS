#ifndef __SYSTEM_H_
#define __SYSTEM_H_

static unsigned long __force_order;

static inline unsigned long native_read_cr3(void)
{
	unsigned long val;	
	asm volatile("mov %%cr3,%0\n\t":"=r"(val),"=m"(__force_order));
	return val;
}


static inline void native_write_cr3(unsigned long val)
{

	asm volatile("mov %0,%%cr3"::"r"(val),"m"(__force_order));	

}
#endif
