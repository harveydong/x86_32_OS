#include <asm/rwlock.h>
#include <asm/atomic.h>

asm(

".align 4\n\t"
".globl __write_lock_failed\n\t"
"__write_lock_failed:\n\t"
"	"LOCK "addl $"RW_LOCK_BIAS_STR ",(%eax)\n\t"
"1: cmpl $"RW_LOCK_BIAS_STR ",(%eax)\n\t"
"	jne 1b\n\t"
"	"LOCK "subl $"RW_LOCK_BIAS_STR ",(%eax)\n\t"
"	jnz __write_lock_failed\n\t"
"	ret"

);

asm(
"	.align 4\n\t"
"	.global __read_lock_failed\n\t"
"__read_lock_failed:\n\t"
"	lock;incl (%eax)\n\t"
"1:	\n\t"	
"	cmpl $1,(%eax)\n\t"
"	js 1b\n\t"
"	lock; decl (%eax)\n\t"
"	js __read_lock_failed\n\t"
"	ret"


);
