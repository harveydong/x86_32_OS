#ifndef __MSR_H_
#define __MSR_H_
#include "init.h"
#include "types.h"

#define EAX_EDX_RET(val,low,high) "=a"(low),"=d"(high)
#define EAX_EDX_VAL(val,low,high) ((low) | ((u64)(high) << 32))
static __always_inline unsigned long long __native_read_tsc(void)
{
	unsigned low,high;
	asm volatile("rdtsc":EAX_EDX_RET(val,low,high));
	
	return EAX_EDX_VAL(val,low,high);
}
#endif
