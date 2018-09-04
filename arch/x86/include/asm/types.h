#ifndef __TYPES_H_
#define __TYPES_H_
#include <linux/posix_types.h>

typedef __kernel_size_t size_t;

typedef unsigned long long __u64; 
typedef unsigned int __u32;
typedef unsigned short __u16;
typedef unsigned char __u8;


typedef signed int __s32;
typedef signed short __s16;
typedef signed char __s8;
typedef long long __s64;


typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned long long u64;





typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int64;

typedef unsigned int addr_t;
typedef unsigned int size_t;

#define bool int
#define NULL ((void*)0)


#endif
