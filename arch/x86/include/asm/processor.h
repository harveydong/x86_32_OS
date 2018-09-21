#ifndef __PROCESSOR_H_
#define __PROCESSOR_H_
#include <linux/types.h>
#include <asm/cpufeature.h>
#include <asm/processor-flags.h>
#include <linux/threads.h>
#include <asm/segment.h>
#include <asm/desc.h>

extern unsigned long mmu_cr4_features;
#define IO_BITMAP_SIZE 32
#define INVALID_IO_BITMAP_OFFSET 0x8000



typedef struct {
	unsigned long seg;

}mm_segment_t;


struct tss_struct {
	unsigned short	back_link,__blh;
	unsigned long	esp0;
	unsigned short	ss0,__ss0h;
	unsigned long	esp1;
	unsigned short	ss1,__ss1h;
	unsigned long	esp2;
	unsigned short	ss2,__ss2h;
	unsigned long	__cr3;
	unsigned long	eip;
	unsigned long	eflags;
	unsigned long	eax,ecx,edx,ebx;
	unsigned long	esp;
	unsigned long	ebp;
	unsigned long	esi;
	unsigned long	edi;
	unsigned short	es, __esh;
	unsigned short	cs, __csh;
	unsigned short	ss, __ssh;
	unsigned short	ds, __dsh;
	unsigned short	fs, __fsh;
	unsigned short	gs, __gsh;
	unsigned short	ldt, __ldth;
	unsigned short	trace, bitmap;
	unsigned long	io_bitmap[IO_BITMAP_SIZE+1];
	/*
	 * pads the TSS to be cacheline-aligned (size is 0x100)
	 */
	unsigned long __cacheline_filler[5];
};


struct cpuinfo_x86{
	__u8 x86;
	__u8 x86_vendor;
	__u8 x86_model;
	__u8 x86_mask;
	char wp_works_ok;
	char hlt_works_ok;
	char hard_math;
	char rfu;

	int cpuid_level;
	__u32 x86_capability[NCAPINTS];
	char x86_vendor_id[16];
	char x86_model_id[64];
	int x86_cache_size;
};

struct i387_fsave_struct{
	int cwd;
	int swd;
	int twd;
	int fip;
	int fcs;
	int foo;
	int fos;
	int st_space[20];
	int status;
};

struct i387_fxsave_struct{
	unsigned short cwd;
	unsigned short swd;
	unsigned short twd;
	unsigned short fop;
	int fip;
	int fcs;
	int foo;
	int fos;
	int mxcsr;
	int reserved;
	int st_space[32];
	int xmm_space[32];
	int padding[56];
}__attribute__((aligned(16)));

struct i387_soft_struct{
	int cwd;
	int swd;
	int twd;
	int fip;
	int fcs;
	int foo;
	int fos;
	int st_space[20];
	unsigned char ftop,changed,lookahead,no_update,rm,alimit;
//	struct info *info;
	unsigned int entry_eip;

};
union i387_union {

	struct i387_fsave_struct fsave;
	struct i387_fxsave_struct fxsave;
	struct i387_soft_struct soft;
};


struct thread_struct{
	unsigned int esp0;
	unsigned int eip;
	unsigned int esp;
	unsigned int fs;
	unsigned int gs;
	
	unsigned int debugreg[8];
	unsigned int cr2,trap_no,error_code;
	union i387_union i387;
	unsigned int screen_bitmap;
	unsigned int v86flags,v86mask,v86mode,saved_esp0;
	int ioperm;
	unsigned int io_bitmap[IO_BITMAP_SIZE+1];
};


struct task_struct;
struct mm_struct;

#define INIT_THREAD {\
	.esp0 = 0,\
	.eip = 0,\
	.fs = 0,\
	.gs = 0,\
	.debugreg ={[0 ... 7] = 0} ,\
	.cr2 = 0 ,\
	.trap_no = 0,\
	.error_code = 0,\
	.i387 = {{0,},},\
	.screen_bitmap = 0,\
	.v86flags = 0,\
	.vm86mask = 0,\
	.vm86mode = 0,\
	.saved_esp0 = 0,\
	.ioperm = 0,\
	.io_bitmap = {~0,} \
}

#define INIT_MMAP \
{ .vm_mm = &init_mm,\
  .vm_start = 0,\
  .vm_end = 0,\
  .vm_next = NULL,\
  .vm_page_prot = PAGE_SHARED,\
  .vm_flags = VM_READ|VM_WRITE|VM_EXEC,\
  .vm_avl_height = 1,\
  .vm_avl_left = NULL,\
  .vm_avl_right = NULL,\
}

#define X86_VENDOR_INTEL 0
#define X86_VENDOR_CENTAUR 5


extern unsigned long mmu_cr4_features;

static inline void set_in_cr4(unsigned long mask)
{
	mmu_cr4_features |= mask;

	__asm__("movl %%cr4,%%eax\n\t"
		"orl %0,%%eax\n\t"
		"movl %%eax,%%cr4\n"
		::"irg"(mask)
		:"ax");

}


#define TASK_SIZE (PAGE_OFFSET)


#define INIT_TSS {\
	.back_link = 0,\
	.__blh = 0, \
	.esp0 = sizeof(init_stack) + (long)&init_stack,\
	.ss0 = __KERNEL_DS,\
	.__ss0h = 0,\
	.esp1 = 0,\
	.ss1 = 0,\
	.__ss1h = 0,\
	.esp2 = 0,\
	.ss2 = 0,\
	.__ss2h = 0,\
	.__cr3 = 0,\
	.eip = 0,\
	.eflags = 0,\
	.eax = 0,\
	.ecx = 0,\
	.edx = 0,\
	.ebx = 0,\
	.esp = 0,\
	.ebp = 0,\
	.esi = 0,\
	.edi = 0,\
	.es = 0,\
	.__esh = 0,\
	.cs = 0,\
	.__csh = 0,\
	.ss = 0,\
	.__ssh = 0,\
	.ds = 0,\
	.__dsh = 0,\
	.fs = 0,\
	.__fsh = 0,\
	.gs = 0,\
	.__gsh = 0,\
	.ldt = __LDT(0),\
	.__ldth = 0,\
	.trace = 0,\
	.bitmap = INVALID_IO_BITMAP_OFFSET,\
	.io_bitmap = {~0,}\
}
	
extern struct tss_struct init_tss[NR_CPUS];

#define init_stack (init_task_union.stack)



#endif
