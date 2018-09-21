#ifndef __RWLOCK_H_
#define __RWLOCK_H_
#include <asm/atomic.h>

#define RW_LOCK_BIAS 0x01000000
#define RW_LOCK_BIAS_STR "0x01000000"


#define __build_write_lock_ptr(rw,helper)\
	asm volatile(LOCK"subl $"RW_LOCK_BIAS_STR ",(%0)\n\t"\
		"jnz 2f\n"\
		"1:\n"\
		".section .text.lock,\"ax\"\n"\
		"2:\t call "helper "\n\t"\
		"jmp 1b\n"\
		".previous"\
		::"a"(rw):"memory")



#define __build_write_lock(rw,helper) do{\
		__build_write_lock_ptr(rw,helper);\
	}while(0)


#endif
