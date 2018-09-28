#ifndef __APIC_H_
#define __APIC_H_
#include <asm/fixmap.h>
#include <asm/apicdef.h>

#define APIC_BASE (fix_to_virt(FIX_APIC_BASE))


static  inline unsigned long apic_read(unsigned long reg)
{

	return *((volatile unsigned long*)(APIC_BASE + reg));
}

static inline void apic_write(unsigned long reg,unsigned long v)
{
	*((volatile unsigned long*)(APIC_BASE+reg)) = v;

}
extern void init_apic_mapping(void);

#define apic_write_around(x,y) apic_write((x),(y))

static  inline void ack_APIC_irq(void)
{
	apic_write_around(APIC_EOI,0);
}
#endif
