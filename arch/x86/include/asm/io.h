#ifndef __ASM_IO_H_
#define __ASM_IO_H_
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

#if 0
static inline void outb(unsigned char value,unsigned short port)
{
	__asm__ __volatile__("outb %b0,%w1"::"a"(value),"Nd"(port));
}
#endif

static inline unsigned char inb(unsigned short port)
{
	unsigned char _v;

	__asm__ __volatile__("inb %w1,%0":"=a"(_v):"Nd"(port));
	return _v;

}
static inline unsigned char inb_p(unsigned char port)
{
	unsigned char _v;
	__asm__ __volatile__("inb %w1,%0" \
			"\noutb %%al,$0x80"\
			:"=a"(_v):"Nd"(port));

	return _v;
}

static inline void outb_p(unsigned char value,unsigned short port)
{
	__asm__ __volatile("outb %b0,%w1"\
			"\noutb %%al,$0x80"\
			::"a"(value),"Nd"(port));
}

static inline void outsb(unsigned short port,const void *addr,unsigned int count)
{
	__asm__ __volatile__("rep;outsb":"=S"(addr),"=c"(count):"d"(port),"0"(addr),"1"(count));
}

static inline void outsw(unsigned short port ,const void *addr,unsigned int count)
{
	__asm__ __volatile__("rep;outsw":"=S"(addr),"=c"(count):"d"(port),"0"(addr),"1"(count));
}

static inline void outsl(unsigned short port ,const void *addr,unsigned int count)
{
	__asm__ __volatile__("rep;outsl":"=S"(addr),"=c"(count):"d"(port),"0"(addr),"1"(count));
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
