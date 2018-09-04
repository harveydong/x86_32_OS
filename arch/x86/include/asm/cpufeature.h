#ifndef __CPUFEATURE_H_
#define __CPUFEATURE_H_

#define X86_FEATURE_XMM (0*32+25)
#define X86_FEATURE_XMM2 (0*32+26)
#define X86_FEATURE_LM		(1*32+29)

#define X86_FEATURE_FPU		(0*32+ 0)
#define X86_FEATURE_PAE		(0*32+ 6)
#define X86_FEATURE_CX8		(0*32+ 8) 
#define X86_FEATURE_CMOV	(0*32+15)
#define X86_FEATURE_3DNOW	(1*32+31)
#define X86_FEATURE_NOPL	(3*32+20)

#define X86_FEATURE_PSE		(0*32+ 3)
#define X86_FEATURE_PGE		(0*32+13)
#define X86_FEATURE_MSR		(0*32+ 5)
#define X86_FEATURE_FXSR	(0*32+24)

#define NCAPINTS 4

#endif
