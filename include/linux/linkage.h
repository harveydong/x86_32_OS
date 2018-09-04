#ifndef __LINKAGE_H_
#define __LINKAGE_H_

#define CPP_ASMLINKAGE 
#define asmlinkage CPP_ASMLINKAGE __attribute__((regparm(0)))

#define SYMBOL_NAME_LABEL(X)  X##:
#define SYMBOL_NAME(X) X

#define __ALIGN .align 4,0x90
#define __ALIGN_STR ".align 4,0x90"

#ifdef __ASSEMBLY__

#define ALIGN __ALIGN
#define ENTRY(name) .globl SYMBOL_NAME(name);\
	ALIGN;\
	SYMBOL_NAME_LABEL(name)
#endif

#endif
