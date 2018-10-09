#ifndef __CACHE_H_
#define __CACHE_H_

#define L1_CACHE_BYTES 64
#define SMP_CACHE_BYTES L1_CACHE_BYTES

#define L1_CACHE_ALIGN(x) (((x) + (L1_CACHE_BYTES - 1)) & ~(L1_CACHE_BYTES - 1))

#define ____cacheline_aligned __attribute__((__aligned__(SMP_CACHE_BYTES)))

#endif
