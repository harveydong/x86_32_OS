#ifndef __ARCH_STRING_H_
#define __ARCH_STRING_H_
#include "types.h"

void *memset(void *s,int c,size_t n);

void *memcpy(void *to,const void *from,size_t len);
#endif
