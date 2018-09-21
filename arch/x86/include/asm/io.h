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

#if 1
static inline void *phys_to_virt(unsigned long address)
{

	return  __va(address);
}
#endif
static  inline unsigned long virt_to_phys(volatile void *address)
{
	return __pa(address);

}
//#define phys_to_virt(address) __va(address)

#define virt_to_bus virt_to_phys
#define bus_to_virt phys_to_virt

#endif
