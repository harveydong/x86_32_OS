#include <asm/bootparam.h>
#include <asm/boot.h>
#include <asm/header_32.h>
struct boot_params boot_params __attribute__((aligned(16)));
char *HEAP = _end;
char *heap_end = _end;


void copy_boot_params()
{
	memcpy(&boot_params.hdr,&hdr,sizeof(hdr));
		
}


static void init_heap(void)
{
	char *stack_end;
	if(boot_params.hdr.loadflags & CAN_USE_HEAP){
		asm("leal %P1(%%esp),%0"
			:"=r"(stack_end):"i"(-STACK_SIZE));
		heap_end = (char *)((size_t)boot_params.hdr.heap_end_ptr+0x200);
		if(heap_end > stack_end)
			heap_end = stack_end;
	}else{
	//	my_puts("WARNING:Ancient bootloader,some funcitonlity may be limited\n");
	
	}
}
static void set_bios_mode(void)
{
	struct biosregs ireg;
	initregs(&ireg);
	
	ireg.ax = 0xec00;
	ireg.bx = 2;
	intcall(0x15,&ireg,NULL);
}

void main(void)
{
	copy_boot_params();

	init_heap();	
	set_bios_mode();
	detect_memory();
	set_video();
	my_puts("ok,now will be while\n");	
	go_to_protected_mode();
	while(1)
	  ;
}
