#ifndef __UACCESS_H_
#define __UACCESS_H_
#include "processor.h"

#define MAKE_MM_SEG(s) ((mm_segment_t) { (s)})
#define KERNEL_DS MAKE_MM_SEG(-1UL)
#endif
