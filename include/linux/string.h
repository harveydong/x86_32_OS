#ifndef __STRING_H_
#define __STRING_H_
#include <linux/types.h>

extern void *memset(void*,int,__kernel_size_t);

extern int memcmp(const void *,const void*,size_t);
extern void *memcpy(void *,const void*,size_t );

extern int strncmp(const char*,const char*,size_t);
#endif
