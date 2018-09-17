#ifndef __FIXMAP_H_
#define __FIXMAP_H_

#include <asm/page.h>
#include <asm/apicdef.h>
#include <asm/kmap_types.h>
#include <linux/threads.h>
#include <asm/pgtable.h>


enum fixed_address{

	FIX_APIC_BASE,
	FIX_IO_APIC_BASE_0,
	FIX_IO_APIC_BASE_END = FIX_IO_APIC_BASE_0+MAX_IO_APICS - 1,
	FIX_KMAP_BEGIN,
	FIX_KMAP_END = FIX_KMAP_BEGIN+(KM_TYPE_NR*NR_CPUS) - 1,
	__end_of_fixed_address
};

#define FIXADDR_TOP (0xFFFFE000UL)
#define FIXADDR_SIZE (__end_of_fixed_address << PAGE_SHIFT)
#define FIXADDR_START (FIXADDR_TOP - FIXADDR_SIZE)



extern void __set_fixmap(enum fixed_address idx,unsigned long phys,pgprot_t flags);

#define __fix_to_virt(x) (FIXADDR_TOP - ((x) << PAGE_SHIFT))


#define set_fixmap_nocache(idx,phys)\
	__set_fixmap(idx,phys,PAGE_KERNEL_NOCACHE)



//extern void __this_fixmap_does_not_exist(void);


static inline unsigned long fix_to_virt(const unsigned int idx)
{
	if(idx >= __end_of_fixed_address)
	{
	
			return;

		//	__this_fixmap_does_not_exist();
		
	}
	
	return __fix_to_virt(idx);
}


#endif
