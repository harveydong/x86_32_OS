#include <asm/types.h>
#include <asm/kernel.h>
#include <asm/elf.h>
#include <asm/loader.h>

extern void puts(char *s);
extern void print_hex(unsigned long long val);
extern void println(void);

void memcpy(void *dst,void *src,uint32 len)
{
	char *tmp_dst = (char *)dst;
	char *tmp_src = (char *)src;
	while(len--){
		(*tmp_dst) = (*tmp_src);
		tmp_dst++;
		tmp_src++;
	}
}

void memset(void*s,int c,uint32 len)
{
	int i;
	char *ss = (char *)s;
	
	for(i = 0; i < len; i++)
		ss[i] = c;
}
void read_sector(void *buf, uint32 lba)
{
	
	bios_read(lba);
	memcpy(buf,(void *)TMP_ADDR,512);
}

void read_segment(void *pa,uint32 offset,uint32 size)
{
	uint8 *p = (uint8 *)pa - (offset % 512);
	uint32 lba = offset/512;
	uint8 *end = p + size;

	for(; p < end; p+=512,lba++){
		read_sector(p,lba);
	}
}
void loadmain()
{
	char buf[512] = {0};
	int i;

//	char *p = 0xb8000;
	
//	for(i = 0; i < 100;i += 2)	
//		*(p+i) = i;
//		((void *)(0xb8000 + i)) = i;
//	puts("loader before physical header\n");


/*read　head_64.bin ,and from convert 32bit to 64bit,size is 10 num sectors from 70 ,0x8c00, for head_64.bin*/
//	read_segment((void *)0x100000,0x8c00,0x1400);

	Elf64_Ehdr *elf = (Elf64_Ehdr*)buf;
/*包括32位的代码,从head32.S开始的,没有了从32位到64位的转换,直接到32位代码*/	
	read_sector(elf,KERNEL_ELF_LBA);
	if(elf->e_ident[0] != 0x7f || elf->e_ident[1] != 'E' || elf->e_ident[2] != 'L' || elf->e_ident[3] != 'F'){
		return;
	}

	uint32 elf_offset = SECT_SIZE * KERNEL_ELF_LBA;

//	print_hex(elf->e_phnum);
//	println();	

	Elf64_Phdr *ph = (Elf64_Phdr*)((uint8 *)elf+elf->e_phoff);
	for(i = 0; i < elf->e_phnum;i++)
	//for(i = 0; i < 6;i++)
	{
//		print_hex(i);
//		println();
//		print_hex(ph->p_paddr);	
//		println();

		if(ph->p_type == PT_LOAD && ph->p_paddr){
		
		//	print_hex(ph->p_paddr);	
		//	println();
			read_segment((void*)(long)(ph->p_paddr),elf_offset + ph->p_offset,ph->p_filesz);
			if(ph->p_memsz > ph->p_filesz){
				memset((void*)(long)ph->p_paddr+ph->p_offset,0,ph->p_memsz - ph->p_filesz);
			}
		}
		ph++;
	}	

//	puts("done for read kernel\n");
//	while(1);
	change_to_real_header();
	
//	kernel_entry_t entry = (kernel_entry_t)elf->e_entry;
//	entry();
}
