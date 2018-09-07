#ifndef __IO_H_
#define __IO_H_
#include <asm/page.h>

static inline unsigned short readw(const volatile void *addr)
{
	unsigned short ret;
	
	asm volatile("movw %1,%0":"=r"(ret):"m"(*(volatile unsigned short *)addr):"memory");

	return ret;

}
static inline void writew(unsigned short val,volatile void *addr)
{
	asm volatile("movw %0,%1"::"r"(val),"m"(*(volatile unsigned short *)addr):"memory");
}


#define IO_SPACE_LIMIT 0xffff

static inline void *phys_to_virt(unsigned long address)
{
	__va(address);
}
static  inline unsigned long virt_to_phys(volatile void *address)
{
	return __pa(address);

}
#define virt_to_bus virt_to_phys

#endif
