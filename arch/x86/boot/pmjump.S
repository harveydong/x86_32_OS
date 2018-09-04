#include <asm/segment.h>
#include <asm/processor-flags.h>

	.text
	.code16	
	.globl protected_mode_jump
protected_mode_jump:
	movl %edx,%esi
	xorl %ebx,%ebx
	movw %cs,%bx	
	shll $4,%ebx
	addl %ebx,2f
	jmp 1f
1:
	movw $__BOOT_DS,%cx
	movw $__BOOT_TSS,%di
	movl %cr0,%edx
	orb $X86_CR0_PE,%dl
	movl %edx,%cr0

	.byte 0x66,0xea
2:	.long in_pm32
	.word __BOOT_CS

	.type protected_mode_jump,@function
	.size protected_mode_jump,.-protected_mode_jump

	
	.code32
	.section ".text32","ax"

	.globl in_pm32
in_pm32:
	movl %ecx,%ds
	movl %ecx,%es
	movl %ecx,%fs
	movl %ecx,%gs
	movl %ecx,%ss

	addl %ebx,%esp
	ltr %di

	xorl %ecx,%ecx
	xorl %edx,%edx
	xorl %ebx,%ebx
	xorl %ebp,%ebp
	xorl %edi,%edi
	
	lldt %cx
	jmpl *%eax

	.type in_pm32,@function
	.size in_pm32,.-in_pm32 
