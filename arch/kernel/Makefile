
OBJS=./head_64.o
all: kernel_64.elf

head64.o:head64.c
	gcc -fno-stack-protector -mcmodel=large -fno-builtin -c head64.c

common.o: common.c
	gcc -fno-builtin -c common.c 

init_task.o: init_task.c
	gcc -c init_task.c
init-mm.o:init-mm.c
	gcc -c init-mm.c

head_64.o: head_64.S percpu.h
	gcc -mcmodel=medium -c head_64.S
init_64.o: init_64.c
	gcc -mcmodel=medium -c init_64.c

string.o: string.S
	gcc -mcmodel=medium -c string.S

traps.o: traps.c
	gcc -fno-builtin  -mcmodel=medium -c traps.c

early_printk.o: early_printk.c
	gcc -fno-builtin -mcmodel=medium -fno-stack-protector -c early_printk.c

physaddr.o: physaddr.c
	gcc -fno-builtin -mcmodel=medium -fno-stack-protector -c physaddr.c
setup_percpu.o:setup_percpu.c
	gcc -fno-builtin -mcmodel=medium -fno-stack-protector -c setup_percpu.c

start_kernel.o: start_kernel.c percpu_fun.h current.h
	gcc -Wall  -mcmodel=large -Wstrict-prototypes -O2 -fomit-frame-pointer  -Werror-implicit-function-declaration  -Wno-format-security -fno-delete-null-pointer-checks -fstrict-aliasing -fno-common -c start_kernel.c
	
kernel_64.elf:  head_64.o head64.o common.o init_task.o init-mm.o  sched.h desc.h percpu.h string.o init_64.o start_kernel.o physaddr.o setup_percpu.o traps.o early_printk.o current.h percpu_fun.h
	ld  -T vmlinux.ld -o $@ head_64.o head64.o start_kernel.o init_task.o init-mm.o string.o  init_64.o  physaddr.o setup_percpu.o traps.o  common.o early_printk.o


clean:
	rm *.o *.elf	
