#ifndef __PTRACE_H_
#define __PTRACE_H_



#define ORIG_EAX 11

#define ES 8
#define DS 7


#ifndef __ASSEMBLY__
#include <asm/vm86.h>

struct pt_regs {
	long ebx;
	long ecx;
	long edx;
	long esi;
	long edi;
	long ebp;
	long eax;
	int  xds;
	int  xes;
	long orig_eax;
	long eip;
	int  xcs;
	long eflags;
	long esp;
	int  xss;
};
#endif


#ifdef __KERNEL__
#define user_mode(regs) ((VM_MASK & (regs)->eflags) || (3 &(regs)->ecx))

#endif
#endif
