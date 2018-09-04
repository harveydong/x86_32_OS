#include "page_64_types.h"
#include "pgtable.h"
#include "processor-flags.h"
#include "msr.h"
#include "../../../segment.h"
#include "percpu.h"

#define GET_CR2_INFO_RCX movq %cr2,%rcx

#define pud_index(x) (((x) >> PUD_SHIFT) &(PTRS_PER_PUD - 1))
#define pgd_index(address) (((address) >> PGDIR_SHIFT) & (PTRS_PER_PGD - 1))


L4_PAGE_OFFSET = pgd_index(__PAGE_OFFSET)
L3_PAGE_OFFSET = pud_index(__PAGE_OFFSET)
L4_START_KERNEL = pgd_index(__START_KERNEL_map)
L3_START_KERNEL = pud_index(__START_KERNEL_map)

	.text
	.section ".head.text","ax"
	.code64
	.globl startup_64
startup_64:
	leaq _text(%rip),%rbp
	subq $_text - __START_KERNEL_map,%rbp
	
	addq %rbp,init_level4_pgt+0(%rip)
	addq %rbp,init_level4_pgt +(L4_PAGE_OFFSET*8)(%rip)
	addq %rbp,init_level4_pgt + (L4_START_KERNEL*8)(%rip)
	
	addq %rbp,level3_ident_pgt + 0(%rip)
	addq %rbp,level3_kernel_pgt + (510*8)(%rip)
	addq %rbp,level3_kernel_pgt +(511*8)(%rip)
	
	addq %rbp,level2_fixmap_pgt +(506*8)(%rip)

	leaq _text(%rip),%rdi
	andq $PMD_PAGE_MASK,%rdi
	
	movq %rdi,%rax
	shrq $PUD_SHIFT,%rax
	andq $(PTRS_PER_PUD - 1),%rax
	jz ident_complete
	
//	leaq (level2_spare_pgt - __START_KERNEL_map + _KERNEL_TABLE)(%rip),%rdx
//	leaq level3_ident_pgt(%rip),%rbx
//	movq %rdx,0(%rbx,%rax,8)

	movq %rdi,%rax //rdi is _text address
	shrq $PMD_SHIFT,%rax
	andq $(PTRS_PER_PMD - 1),%rax
	leaq __PAGE_KERNEL_IDENT_LARGE_EXEC(%rdi),%rdx
//	leaq level2_spare_pgt(%rip),%rbx
//	movq %rdx,0(%rbx,%rax,8)
ident_complete:
	leaq level2_kernel_pgt(%rip),%rdi
	leaq 4096(%rdi),%r8
	
1:
	testq $1,0(%rdi)
	jz 2f
	addq %rbp,0(%rdi)
2:
	addq $8,%rdi
	cmp %r8,%rdi
	jne 1b


	addq %rbp,phys_base(%rip)
	jmp secondary_startup_64
.globl secondary_startup_64
secondary_startup_64:
	movq $(init_level4_pgt - __START_KERNEL_map),%rax
	addq phys_base(%rip),%rax
	movq %rax,%cr3
	
	movq $1f,%rax
	jmp *%rax
1:
	movl $0x80000001,%eax
	cpuid
	movl %edx,%edi
	
	movl $MSR_EFER,%ecx
	rdmsr
	btsl $_EFER_SCE,%eax
	btl $20,%edi
	jnc 1f
	btsl $_EFER_NX,%eax
1:
	wrmsr


	movl $(X86_CR4_PAE|X86_CR4_PGE),%eax
	movq %rax,%cr4


	movl $CR0_STATE,%eax
	movq %rax,%cr0
	


	movq stack_start(%rip),%rsp
	pushq $0
	popfq

	lgdt early_gdt_descr(%rip)
	
	xorl %eax,%eax
	movl %eax,%ds
	movl %eax,%ss
	movl %eax,%es

	movl %eax,%fs
	movl %eax,%gs
	
	movl $MSR_GS_BASE,%ecx
	movq initial_gs(%rip),%rax
	movq %rax,%rdx
	shrq $32,%rdx
	wrmsr
	
	movl %esi,%edi
	movq initial_code(%rip),%rax
	pushq $0
	pushq $__KERNEL_CS
	pushq %rax
	retq
		



	.section ".ref.data","aw"
	.align 8
	.globl initial_code
initial_code:
	.quad x86_64_start_kernel

	.globl initial_gs
initial_gs:
	.quad  INIT_PER_CPU_VAR(irq_stack_union)
	
	.globl stack_start
stack_start:
	.quad init_thread_union + THREAD_SIZE - 8
	.word 0
	
	.previous


	.section ".init.text","ax"
	.globl early_idt_handlers
early_idt_handlers:
	i = 0	
	.rept 256
	movl $i,%esi
	jmp early_idt_handler
	i = i+1;
	.endr

.globl early_idt_handler
.p2align 4,0x90
early_idt_handler:
	cmpl $2,early_recursion_flag(%rip)
	jz 1f
	incl early_recursion_flag(%rip)
	GET_CR2_INFO_RCX
	movq %rcx,%r9
	xorl %r8d,%r8d
	movl %esi,%ecx
	cmpl $31,%ecx
	ja 0f
	movl $1,%eax
	salq %cl,%rax
	testl $0x27d00,%eax
	je 0f
	popq %r8
0:
	movq 0(%rsp),%rcx
	movq 8(%rsp),%rdx
	xorl %eax,%eax
	leaq early_idt_msg(%rip),%rdi
	call early_printk
	cmpl $2,early_recursion_flag(%rip)
	jz 1f
//	call dump_stack
	
1:
	hlt
	jmp 1b

early_recursion_flag:
	.long 0
early_idt_msg:
	.asciz "Panic: early exception %021x rip %lx:%lx error %lx cr2 %lx\n"

	.previous	

#define PMDS(START,PERM,COUNT)\
	i = 0;\
	.rept (COUNT);\
	.quad (START) + (i << PMD_SHIFT) +(PERM);\
	i = i + 1;\
	.endr



	
#define NEXT_PAGE(name) .balign PAGE_SIZE;\
.globl name;\
.align 4,0x90;\
name:
	

	.data


.balign PAGE_SIZE
.globl init_level4_pgt
.align 4,0x90
init_level4_pgt:
	.quad level3_ident_pgt - 0xffffffff80000000  + _KERNPG_TABLE

	.org  init_level4_pgt + L4_PAGE_OFFSET*8,0
	.quad level3_ident_pgt - __START_KERNEL_map + _KERNPG_TABLE
	
	.org  init_level4_pgt + L4_START_KERNEL*8,0 
	.quad level3_kernel_pgt - __START_KERNEL_map + _PAGE_TABLE

.balign PAGE_SIZE
.globl level3_ident_pgt
.align 4,0x90
level3_ident_pgt:
	.quad level2_ident_pgt - __START_KERNEL_map + _KERNPG_TABLE
	.fill 511,8,0
	
NEXT_PAGE(level3_kernel_pgt)
	.fill L3_START_KERNEL,8,0
	.quad level2_kernel_pgt - __START_KERNEL_map + _KERNEL_TABLE
	.quad level2_fixmap_pgt - __START_KERNEL_map + _PAGE_TABLE

NEXT_PAGE(level2_fixmap_pgt)
	.fill 506,8,0
	.quad level1_fixmap_pgt - __START_KERNEL_map+_PAGE_TABLE
	.fill 5,8,0
NEXT_PAGE(level1_fixmap_pgt)
	.fill 512,8,0
	
NEXT_PAGE(level2_ident_pgt)
	PMDS(0,__PAGE_KERNEL_IDENT_LARGE_EXEC,PTRS_PER_PMD)
	
NEXT_PAGE(level2_kernel_pgt)
	PMDS(0,__PAGE_KERNEL_LARGE_EXEC,(512*1024*1024)/(2*1024*1024))

//NEXT_PAGE(level2_spare_pgt)
//	.fill 512,8,0

#undef PMDS
#undef NEXT_PAGE

	.data
	.align 16
	.globl early_gdt_descr
early_gdt_descr:
	.word GDT_ENTRIES*8 - 1
early_gdt_descr_base:
	.quad INIT_PER_CPU_VAR(gdt_page)		

.globl phys_base
	.p2align 4,0x90
phys_base:
	.quad 0x0000000000000000


	.section .bss,"aw",@nobits
	.align 64
	.globl idt_table
	
idt_table:
	.skip IDT_ENTRIES*16

	.section ".bss..page_aligned","aw"

	.align PAGE_SIZE
	.globl empty_zero_page
empty_zero_page:
	.skip PAGE_SIZE
