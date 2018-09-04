.text

#include <linux/linkage.h>
#include <asm/page.h>
#include <asm/processor-flags.h>
#include <asm/pgtable.h>
#include <linux/threads.h>
#include <asm/segment_head.h>
#include <asm/desc.h>


#define OLD_CL_MAGIC_ADDR 0x90020
#define OLD_CL_MAGIC 0xA33F
#define OLD_CL_BASE_ADDR 0x90000
#define OLD_CL_OFFSET 0x90022
#define NEW_CL_POINTER 0x228

#define CPU_PARAMS SYMBOL_NAME(boot_cpu_data)
#define X86 CPU_PARAMS+0
#define	X86_VENDOR	CPU_PARAMS+1
#define X86_MODEL	CPU_PARAMS+2
#define X86_MASK	CPU_PARAMS+3
#define X86_CPUID	CPU_PARAMS+8
#define X86_CAPABILITY	CPU_PARAMS+12
#define	X86_VENDOR_ID	CPU_PARAMS+16

ENTRY(stext)
ENTRY(_stext)
startup_32:
	cld
	movl $(__KERNEL_DS),%eax
	movl %eax,%ds
	movl %eax,%es
	movl %eax,%fs
	movl %eax,%gs
	
	orw %bx,%bx
	jz 1f

#define cr4_bits  mmu_cr4_features - __PAGE_OFFSET
	movl cr4_bits,%edx
	andl %edx,%edx
	jz 1f
	movl %cr4,%eax
	orl %edx,%eax
	movl %eax,%cr4

1:
	movl $swapper_pg_dir - __PAGE_OFFSET,%edi
	movl $0x00102007,%eax
	stosl


	/*initialize page tables */
	movl $pg0 - __PAGE_OFFSET,%edi
	movl $0x007,%eax
2:
	stosl
	addl $0x1000,%eax
	cmpl $empty_zero_page - __PAGE_OFFSET,%edi
	jnz 2b

3:
	movl $swapper_pg_dir - __PAGE_OFFSET,%eax
	movl %eax,%cr3
	movl %cr0,%eax
	orl $0x80000000,%eax
	movl %eax,%cr0

	jmp 1f
1:

	movl $1f,%eax

	jmp *%eax
1:



#	lss　stack_start,%esp

	movl (stack_start),%esp

	orw %bx,%bx
	jz 1f
	pushl $0
	popfl
	jmp checkCPUtype
	
1:

	xorl %eax,%eax
	movl $SYMBOL_NAME(__bss_start),%edi
	movl $SYMBOL_NAME(_end),%ecx
	subl %edi,%ecx
	rep
	stosb

	call setup_idt
	pushl $0
	popfl
	
	movl $SYMBOL_NAME(empty_zero_page),%edi
	movl $512,%ecx
	cld
	rep
	movsl
	xorl %eax,%eax
	mov $512,%ecx
	rep
	stosl

	movl SYMBOL_NAME(empty_zero_page) + NEW_CL_POINTER,%esi
	andl %esi,%esi
	jnz 2f
	cmpw $(OLD_CL_MAGIC),OLD_CL_MAGIC_ADDR
	jne 1f
	movzwl OLD_CL_OFFSET,%esi
	addl $(OLD_CL_BASE_ADDR),%esi

2:
	movl $SYMBOL_NAME(empty_zero_page)+2048,%edi
	movl $512,%ecx
	rep
	movsl

1:
checkCPUtype:
	movl $-1,X86_CPUID
	movl $3,X86
	pushfl
	popl %eax
	movl %eax,%ecx
	xorl $0x40000,%eax
	pushl %eax
	popfl
	pushfl
	popl %eax
	xorl %ecx,%eax
	andl $0x40000,%eax

	movl $4,X86
	movl %ecx,%eax	
	xorl $0x200000,%eax
	pushl %eax
	popfl
	pushfl
	popl %eax
	xorl %ecx,%eax
	pushl %ecx
	popfl
	andl $0x200000,%eax
	
	xorl %eax,%eax
	cpuid
	movl %eax,X86_CPUID
	movl %ebx,X86_VENDOR_ID
	movl %edx,X86_VENDOR_ID+4
	movl %ecx,X86_VENDOR_ID+8

	orl %eax,%eax
	movl $1,%eax
	cpuid
	movb %al,%cl
	andb $0x0f,%ah
	movb %ah,X86
	andb $0xf0,%al
	movb %al,X86_MODEL
	andb $0x0f,%cl
	movb %cl,X86_MASK
	movl %edx,X86_CAPABILITY
is486:
	movl %cr0,%eax
	andl $0x80000011,%eax
	orl $0x50022,%eax
	jmp 2f
2:
	movl %eax,%cr0
	#call check_x87
	incb ready
	
	lgdt gdt_descr
	lidt idt_descr
	ljmp $(__KERNEL_CS),$1f
1:
	movl $(__KERNEL_DS),%eax
	movl %eax,%ds
	movl %eax,%es
	movl %eax,%fs
	movl %eax,%gs
	
	movl %eax,%ss
	xorl %eax,%eax
	lldt %ax
	cld
	movb ready,%cl
	cmpb $1,%cl
	je 1f
	
	call initialize_secondary
	jmp L6
1:

	call start_kernel
L6:
	jmp L6

ready:
	.byte 0


setup_idt:
	lea ignore_int,%edx
	movl $(__KERNEL_CS<<16),%eax
	movw %dx,%ax
	movw $0x8E00,%dx
	
	lea idt_table,%edi
	mov $256,%ecx
rp_sidt:
	movl %eax,(%edi)
	movl %edx,4(%edi)
	addl $8,%edi
	dec %ecx
	jne rp_sidt
	ret

#ENTRY(stack_start)
	.globl stack_start
	ALIGN
stack_start:
	.long init_task_union + 8192
	.long __KERNEL_DS

int_msg:
	.asciz "Unkown interrupt by harveydong\n"
	ALIGN

ignore_int:
	cld
	pushl %eax
	pushl %ecx
	pushl %edx
	pushl %es
	pushl %ds
	movl $(__KERNEL_DS),%eax
	movl %eax,%ds
	movl %eax,%es
	pushl $int_msg
#	call SYMBOL_NAME(printk)
	popl %eax
	popl %ds
	popl %edx
	popl %ecx
	popl %eax
	iret


#define IDT_ENTRIES 256
#define GDT_ENTRIES (__TSS(NR_CPUS))

.globl idt
.globl gdt

	ALIGN
	.word 0

idt_descr:
	.word IDT_ENTRIES*8-1
idt:
	.long idt_table
	.word 0

gdt_descr:
	.word GDT_ENTRIES*8-1
gdt:	
	.long gdt_table
	

	.org 0x1000
	.globl swapper_pg_dir
swapper_pg_dir:
	.long 0x00102007
	.long 0x00103007
	.fill BOOT_USER_PGD_PTRS - 2,4,0
	
	.long 0x00102007
	.long 0x00103007
	.fill BOOT_KERNEL_PGD_PTRS - 2,4,0

	.org 0x2000
ENTRY(pg0)
	
	.org 0x3000
ENTRY(pg1)

	.org 0x4000
ENTRY(empty_zero_page)

	.org 0x5000
ENTRY(empty_bad_page)

	.org 0x6000
ENTRY(empty_bad_pte_table)

	.org 0x7000
ENTRY(empty_bad_pmd_table)
	
	.org 0x8000

.data
	ALIGN

ENTRY(gdt_table)
	.quad 0x0000000000000000
	.quad 0x0000000000000000
	.quad 0x00cf9a000000ffff /*0x10 kernel 4GB code at 0x00000000*/
	.quad 0x00cf92000000ffff /*0x18 kernel 4GB data at 0x000000*/
	.quad 0x00cffa000000ffff /*0x23 user 4GB code at 0x000*/
	.quad 0x00cff2000000ffff /*0x2b user 4GB data at 0x000*/
	.quad 0x0000000000000000
	.quad 0x0000000000000000

	.quad 0x0040920000000000 /*0x40 APM set up for bad BIOS's*/
	.quad 0x00409a0000000000 /*0x48 APM CS code*/
	.quad 0x00009a0000000000 /*0x50 APM CS 16 code*/
	.quad 0x0040092000000000 /*0x58 APM DS data*/
	.fill NR_CPUS*4,8,0     /*space for TSS's and LDT's*/


