#ifndef __TLBFLUSH_H_
#define __TLBFLUSH_H_
#include "p_types.h"
#include "system.h"


static inline void __native_flush_tlb(void)
{
	native_write_cr3(native_read_cr3());

}
#define __flush_tlb()  __native_flush_tlb()

static inline void __flush_tlb_all(void)
{
	__flush_tlb();
}

#endif
