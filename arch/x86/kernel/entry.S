#include <linux/linkage.h>
#include <asm/segment.h>
#include <linux/sys.h>


DS=0x1C
ES=0x20
EAX=0x18
ORIG_EAX=0x24
CS=0x2c
EFLAGS=0x30
VM_MASK=0x00020000

processor=52
need_resched=20
sigpending=8
tsk_ptrace=24

ENOSYS=38

#define SAVE_ALL \
	cld; \
	pushl %es; \
	pushl %ds; \
	pushl %eax; \
	pushl %ebp; \
	pushl %edi; \
	pushl %esi; \
	pushl %edx; \
	pushl %ecx; \
	pushl %ebx; \
	movl $(__KERNEL_DS),%edx; \
	movl %edx,%ds; \
	movl %edx,%es;





#define RESTORE_ALL\
	popl %ebx;\
	popl %ecx;\
	popl %edx;\
	popl %esi;\
	popl %edi;\
	popl %ebp;\
	popl %eax;\
1:	popl %ds;\
2:	popl %es;\
	addl $4,%esp;\
3:	iret;\
	.section .fixup,"ax";\
4: 	movl $0,(%esp);\
	jmp 1b;\
5:	movl $0,(%esp);\
	jmp 2b;\
6:	pushl %ss;\
	popl %ds;\
	pushl %ss;\
	popl %es;\
	pushl $11;\
	call do_exit;\
.previous;\
	.section __ex_table,"a";\
	.align 4;\
	.long 1b,4b;\
	.long 2b,5b;\
	.long 3b,6b;\
.previous

#define GET_CURRENT(reg) \
	movl $-8192,reg;\
	andl %esp,reg


ENTRY(system_call)
	pushl %eax
	SAVE_ALL
	GET_CURRENT(%ebx)
	cmpl $(NR_syscalls),%eax
	jae badsys
	testb $0x02,tsk_ptrace(%ebx)
	jne tracesys
	call *SYMBOL_NAME(sys_call_table)(,%eax,4)	
	movl %eax,EAX(%esp)
	
	
ENTRY(ret_from_sys_call)
	movl processor(%ebx),%eax
	shll $8,%eax
	movl SYMBOL_NAME(irq_stat)(,%eax),%ecx
	testl SYMBOL_NAME(irq_stat)+4(,%eax),%ecx
	jne handle_softirq





ret_with_reschedule:
	cmpl $0,need_resched(%ebx)
	jne reschedule
	cmpl $0,sigpending(%ebx)
	jne signal_return

restore_all:
	RESTORE_ALL


	ALIGN
signal_return:
	sti



	ALIGN
tracesys:
	movl $-ENOSYS,EAX(%esp)
	
	jmp ret_from_sys_call

badsys:
	movl $-ENOSYS,EAX(%esp)
	
	jmp ret_from_sys_call

	ALIGN
handle_softirq:
	call SYMBOL_NAME(do_softirq)
	jmp ret_from_intr

	ALIGN
reschedule:
	call SYMBOL_NAME(schedule)
	jmp ret_from_sys_call


	ALIGN
ret_from_exception:
	GET_CURRENT(%ebx)
	movl processor(%ebx),%eax
	shll $8,%eax
	movl SYMBOL_NAME(irq_stat)(,%eax),%ecx
	testl SYMBOL_NAME(irq_stat)+4(,%ecx),%ecx
	jne handle_softirq

ENTRY(ret_from_intr)
	GET_CURRENT(%ebx)
	movl EFLAGS(%esp),%eax
	movb CS(%esp),%al
	testl $(VM_MASK|3),%eax
	jne ret_with_reschedule
	jmp restore_all

	

ENTRY(divide_error)
	pushl $0 #no error code
	pushl $SYMBOL_NAME(do_divide_error)
	ALIGN
error_code:
	pushl %ds	
	pushl %eax
	xorl %eax,%eax
	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %edx
	decl %eax	#eax=-1
	pushl %ecx
	pushl %ebx
	cld
	movl %es,%ecx
	movl ORIG_EAX(%esp),%esi #get the error code
	movl ES(%esp),%edi #get the function address
	movl %eax,ORIG_EAX(%esp)
	movl %ecx,ES(%esp)
	movl %esp,%edx
	pushl %esi
	pushl %edx
	movl $(__KERNEL_DS),%edx
	movl %edx,%ds
	movl %edx,%es
	GET_CURRENT(%ebx)
	call *%edi
	addl $8,%esp
	jmp ret_from_exception



ENTRY(debug)
	pushl $0
	pushl $SYMBOL_NAME(do_debug)
	jmp error_code

ENTRY(nmi)
	pushl %eax
	SAVE_ALL
	movl %esp,%edx
	pushl $0
	pushl %edx
	call SYMBOL_NAME(do_nmi)
	addl $8,%esp
	RESTORE_ALL

ENTRY(int3)
	pushl $0
	pushl $SYMBOL_NAME(do_int3)
	jmp error_code

ENTRY(page_fault)
	pushl $SYMBOL_NAME(do_page_fault)
	jmp error_code

.data
ENTRY(sys_call_table)
	.long SYMBOL_NAME(sys_ni_syscall) /*0*/
	.long SYMBOL_NAME(sys_exit)
	
	.rept NR_syscalls - 221
		.long SYMBOL_NAME(sys_ni_syscall)
	.endr
