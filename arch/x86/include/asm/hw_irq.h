#ifndef __HW_IRQ_H_
#define __HW_IRQ_H_
#include <asm/irq.h>
#include <linux/linkage.h>
#include <asm/segment.h>

#define FIRST_EXTERNAL_VECTOR 0x20
#define SYSCALL_VECTOR 0x80


#define RESCHEDULE_VECTOR 0xfc
#define SPURIOUS_APIC_VECTOR 0xff
#define CALL_FUNCTION_VECTOR 0xfb
#define INVALIDATE_TLB_VECTOR 0xfd
#define LOCAL_TIMER_VECTOR 0xef

#define ERROR_APIC_VECTOR 0xfe


#define FIRST_DEVICE_VECTOR 0x31

#define __STR(x) #x
#define STR(x) __STR(x)

#define SAVE_ALL \
	"cld \n\t"\
	"pushl %es \n\t"\
	"pushl %ds \n\t" \
	"pushl %eax \n\t" \
	"pushl %ebp \n\t" \
	"pushl %edi \n\t"\
	"pushl %esi \n\t"\
	"pushl %edx \n\t"\
	"pushl %ecx \n\t" \
	"pushl %ebx \n\t"\
	"movl $"STR(__KERNEL_DS)",%edx\n\t"\
	"movl %edx,%ds\n\t"\
	"movl %edx,%es\n\t"
#define BUILD_SMP_INTERRUPT(x,y) XBUILD_SMP_INTERRUPT(x,y)
#define XBUILD_SMP_INTERRUPT(x,v) \
asmlinkage void x(void);\
asmlinkage void call_##x(void);\
__asm__( \
	"\n"__ALIGN_STR"\n"\
	SYMBOL_NAME_STR(x)":\n\t" \
	"pushl $"#v"\n\t" \
	SAVE_ALL \
	SYMBOL_NAME_STR(call_##x)":\n\t" \
	"call "SYMBOL_NAME_STR(smp_##x)"\n\t" \
	"jmp ret_from_intr\n");


#define BUILD_SMP_TIMER_INTERRUPT(x,v) XBUILD_SMP_TIMER_INTERRUPT(x,v)
#define XBUILD_SMP_TIMER_INTERRUPT(x,v) \
asmlinkage void x(struct pt_regs *regs);\
asmlinkage void call_##x(void);\
__asm__(\
	"\n"__ALIGN_STR"\n"\
	SYMBOL_NAME_STR(x)":\n\t" \
	"pushl $"#v" \n\t" \
	SAVE_ALL \
	"movl %esp,%eax \n\t" \
	"pushl %eax \n\t" \
	SYMBOL_NAME_STR(call_##x)":\n\t" \
	"call "SYMBOL_NAME_STR(smp_##x)"\n\t"\
	"addl $4,%esp \n\t" \
	"jmp ret_from_intr \n");



static inline void x86_do_profile(unsigned long eip)
{

}

extern volatile unsigned long irq_err_count;
#endif
