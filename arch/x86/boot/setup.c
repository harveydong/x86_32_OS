#include <asm/param.h>
#include <asm/boot.h>

#include <linux/types.h>
#include <asm/disk.h>
#include <linux/elf32.h>

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

struct boot_params params __attribute__((aligned(16)));

char *HEAP = _end;
char *heap_end = _end;

#define __head_text __attribute__((section(".head.text")))
#if 0
static void init_heap(void)
{
	char *stack_end;
//	if(params.hdr.loadflags & CAN_USE_HEAP){
	asm("leal %P1(%%esp),%0":"=r"(stack_end):"i"(-STACK_SIZE));

	heap_end = (char *)((size_t)(params.hdr.heap_end_ptr+0x200);
	if(heap_end > stack_end)
			heap_end = stack_end;
	//}else{
	//	my_puts("WARNING:Ancient bootloader,some funcitonlity may be limited\n");
	
	//}
}

#endif




static void load_kernel(struct boot_header *header)
{
	char buf[512] = {0};
	struct disk_drive drive;
	char *load_addr = (char*)header->kernel_load_addr ;
	unsigned block = header->kernel_start_sector;
	unsigned blocks = (1+511)/512;
	int i,j;
	int num;

	Elf32_Ehdr *elf = (Elf32_Ehdr *)buf;

	printf("kernel addr:0x%lx\n",header->kernel_load_addr);	
	fill_disk_drive(header->boot_drive,&drive);
	
	read_block(&drive,block,buf);
	

	printf("elf0:%x,elf1:%x,elf2:%x,elf3:%x\n",elf->e_ident[0],elf->e_ident[1],elf->e_ident[2],elf->e_ident[3]);
	if(elf->e_ident[0] != 0x7f || elf->e_ident[1] != 'E' || elf->e_ident[2] != 'L' || elf->e_ident[3] != 'F'){
		
		puts("read here\n");
		while(1);
	}

	

	uint32 elf_offset = SECT_SIZE * KERNEL_ELF_LBA;

	Elf32_Phdr *ph = (Elf32_Phdr*)((uint8 *)elf + elf->e_phoff);
	
	for(i = 0; i < elf->e_phnum;i++)
	{
 

		if(ph->p_type == PT_LOAD && ph->p_paddr){
		
			num = (ph->p_filesz + 511)/512;
			block = (elf_offset + ph->p_offset + 511)/512;
			load_addr = (char*)ph->p_paddr;
			printf("poffset:%x,and block:%x\n",ph->p_offset,block);	
			for(j = 0; j < num;j++)
			{
				read_block(&drive,block,(load_addr));
				block++;
				load_addr += 512;
			}
			//printf("read ok here\n");
		//	read_segment((void*)(long)(ph->p_paddr),elf_offset + ph->p_offset,ph->p_filesz);
			if(ph->p_memsz > ph->p_filesz){
				memset((void*)(load_addr),0,ph->p_memsz - ph->p_filesz);
			}
		}
		ph++;
	}
	
}




void __head_text  setup(struct boot_header *header)
{
	console_init();
	puts("Booting...\n");

	//copy_boot_params();
	params.boot_header = *header;

	e820_detect(&params);

	enable_a20();
		
		
	load_kernel(&params.boot_header);

	puts("load kernel done...\n");	
//	init_heap();
	set_video();
	go_to_protected_mode();
	while(1);
}
