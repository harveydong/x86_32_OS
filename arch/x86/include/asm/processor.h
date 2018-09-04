#ifndef __PROCESSOR_H_
#define __PROCESSOR_H_
#include <linux/types.h>
#include <asm/cpufeature.h>

extern unsigned long mmu_cr4_features;

typedef struct {
	unsigned long seg;

}mm_segment_t;


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
#endif
