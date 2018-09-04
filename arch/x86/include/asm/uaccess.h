#ifndef __UACCESS_H_
#define __UACCESS_H_
#include <asm/page.h>
#include <asm/processor.h>

#define MAKE_MM_SEG(s) ((mm_segment_t){(s)})

#define KERNEL_DS MAKE_MM_SEG(0xFFFFFFFF)
#define USER_DS MAKE_MM_SEG(PAGE_OFFSET)
#endif
