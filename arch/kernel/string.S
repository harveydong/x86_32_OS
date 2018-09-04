.globl __memcpy
	.p2align 4,0x90
__memcpy:

.globl memcpy
	.p2align 4,0x90
memcpy:

	.cfi_startproc
	movq %rdi,%rax
	movl %edx,%ecx
	shrl $6,%ecx
	jz .Lhandle_tail_mcpy
	.p2align 4
.Lloop_64_mcpy:
	decl %ecx
	movq 0*8(%rsi),%r11
	movq 1*8(%rsi),%r8
	movq %r11,0*8(%rdi)
	movq %r8,1*8(%rdi)
	
	movq 2*8(%rsi),%r9
	movq 3*8(%rsi),%r10
	movq %r9,2*8(%rdi)
	movq %r10,3*8(%rdi)
	
	movq 4*8(%rsi),%r11
	movq 5*8(%rsi),%r8
	movq %r11,4*8(%rdi)
	movq %r8,5*8(%rdi)
	
	movq 6*8(%rsi),%r9
	movq 7*8(%rsi),%r10
	movq %r9,6*8(%rdi)
	movq %r10,7*8(%rdi)
	
	leaq 64(%rsi),%rsi
	leaq 64(%rdi),%rdi
	
	jnz .Lloop_64_mcpy
.Lhandle_tail_mcpy:
	movl %edx,%ecx
	andl $63,%ecx
	shrl $3,%ecx
	jz .Lloop_7_mcpy
	
	.p2align 4
.Lloop_8_mcpy:
	decl %ecx
	movq (%rsi),%r8
	movq %r8,(%rdi)
	leaq 8(%rdi),%rdi
	leaq 8(%rsi),%rsi
	jnz .Lloop_8_mcpy
.Lloop_7_mcpy:
	movl %edx,%ecx
	andl $7,%ecx
	jz .Lend_mcpy
	.p2align 4
	
.Lloop_1_mcpy:
	movb (%rsi),%r8b
	movb %r8b,(%rdi)
	incq %rdi
	incq %rsi	
	decl %ecx
	jnz .Lloop_1_mcpy

.Lend_mcpy:
	ret
	.cfi_endproc
	.type memcpy,@function
	.size memcpy,.-memcpy
	
	.type __memcpy,@function
	.size __memcpy,.-__memcpy
	




/*rdi--destination, 
* rsi--value (char)
* rdx--count (bytes)
*/
.globl memset
	.p2align 4,0x90
memset:
	
.globl __memset
	.p2align 4,0x90
__memset:
	.cfi_startproc
	movq %rdi,%r10
	movq %rdx,%r11
	movzbl %sil,%ecx
	movabs $0x0101010101010101,%rax
	mul %rcx
	
	movl %edi,%r9d
	andl $7,%r9d
	jnz .Lbad_alignment
	.cfi_remember_state
.Lafter_bad_alignment:
	movl %r11d,%ecx
	shrl $6,%ecx
	jz .Lhandle_tail

	.p2align 4
.Lloop_64:
	decl %ecx
	movq %rax,(%rdi)
	movq %rax,8(%rdi)
	movq %rax,16(%rdi)
	movq %rax,24(%rdi)
	movq %rax,32(%rdi)
	movq %rax,40(%rdi)
	movq %rax,48(%rdi)
	movq %rax,56(%rdi)
	
	leaq 64(%rdi),%rdi
	jnz .Lloop_64
	
	.p2align 4
.Lhandle_tail:
	movl %r11d,%ecx
	andl $63&(~7),%ecx
	jz .Lhandle_7
	shrl $3,%ecx
	
	.p2align 4
.Lloop_8:
	decl %ecx
	movq  %rax,(%rdi)
	leaq 8(%rdi),%rdi	
	jnz .Lloop_8
	
.Lhandle_7:
	movl %r11d,%ecx
	andl $7,%ecx
	jz .Lende
	
	.p2align 4
.Lloop_1:	
	decl %ecx
	movb %al,(%rdi)
	leaq 1(%rdi),%rdi
	jnz .Lloop_1
.Lende:
	movq %r10,%rax
	ret
	.cfi_restore_state
.Lbad_alignment:
	cmpq $7,%r11
	jbe .Lhandle_7
	movq %rax,(%rdi)
	movq $8,%r8
	subq %r9,%r8
	addq %r8,%rdi
	subq %r8,%r11
	jmp .Lafter_bad_alignment
.Lfinal:
	.cfi_endproc
	
	.type memset,@function
	.size memset,.-memset
	
	.type __memset,@function
	.size __memset,. - __memset
