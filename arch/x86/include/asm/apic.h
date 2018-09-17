#ifndef __APIC_H_
#define __APIC_H_
#include <asm/fixmap.h>


#define APIC_BASE (fix_to_virt(FIX_APIC_BASE))


static  inline unsigned long apic_read(unsigned long reg)
{

	return *((volatile unsigned long*)(APIC_BASE + reg));
}

extern void init_apic_mapping(void);

#endif
