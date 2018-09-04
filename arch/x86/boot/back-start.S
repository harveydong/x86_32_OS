#include <asm/kernel.h>

	.code16
	.globl _start

_start:
	jmp main


boot_disk:	
	.byte 0

clear_screen:
	movw $0x0600,%ax
	xorw %cx,%cx
	movb $0x07,%bh
	movb $0x18,%dh
	movb $0x4f,%dl
	int $0x10
	movb $0x02,%ah
	movb $0,%bh
	movw $0,%dx
	int $0x10
	ret


disk_addr_packet1:
	.byte 0x10
	.byte 0x00
	.word 0x5
	.word 0x7d00
	.word 0x00
	.long 0x1
	.long 0x00

disk_addr_packet2:
	.byte 0x10
	.byte 0x00
	.word 0x40
	.word 0x8700
	.word 0x00
	.long 0x6
	.long 0x00

load_loader:
	lea disk_addr_packet1,%si
	movb $0x42,%ah
	movb boot_disk,%dl
	int $0x13
	ret
	
load_header:
	lea disk_addr_packet2,%si
	movb $0x42,%ah
	movb boot_disk,%dl
	int $0x13
	ret


main:
	xorw %ax,%ax
	movw %ax,%ds
	movw %ax,%es
	movw %ax,%ss
	movw $0x7c00,%sp
	movb %dl,boot_disk

	call clear_screen
	movw $loader_msg,%si
	call puts

	call load_loader

	movw $header_msg,%si
	call puts
	call load_header
	movw $0x7d00,%ax
	jmp *%ax
	#call begin_protected_mode

1: 
	jmp 1b


loader_msg: .asciz "loading loader...\n"
header_msg: .asciz "\n\rloading header...\n"
puts:
	pusha
	movb $0x0e,%ah
	xorb %bh,%bh
do_print_loop:
	lodsb
	test %al,%al
	jz do_print_done
	int $0x10
	jmp do_print_loop

do_print_done:
	popa
	ret 




	.org 0x1fe,0x90
	.word 0xaa55


