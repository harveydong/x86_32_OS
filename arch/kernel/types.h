#ifndef __TYPES_H_
#define __TYPES_H_
#define NULL 0
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;
typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int64;
typedef signed char __s8;
typedef unsigned char __u8;

typedef unsigned long long __u64;
typedef unsigned short __u16;
typedef unsigned int __u32;

typedef short __s16;
typedef int __s32;
typedef long long __s64;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned long long u64; 

typedef __u64 Elf64_Addr;
typedef __u16 Elf64_Half;
typedef __s16 Elf64_SHalf;
typedef __u64 Elf64_Off;
typedef __s32 Elf64_Sword;
typedef __u32 Elf64_Word;
typedef __u64 Elf64_Xword;
typedef __s64 Elf64_Sxword;

typedef unsigned int size_t;

#define asmlinkage
typedef unsigned int addr_t;
#define bool int

#endif
