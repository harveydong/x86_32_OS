#include <linux/init.h>
#include <asm/mpspec.h>
#include <asm/page.h>
#include <linux/bootmem.h>
#include <asm/io.h>


int smp_found_config;
static struct intel_mp_floating *mpf_found;

static int __init mpf_checksum(unsigned char *mp,int len)
{
	int sum = 0;
	
	while(len--)
		sum += *mp++;
		
	return sum & 0xFF;
}


static int __init smp_scan_config(unsigned long base,unsigned long length)
{

	unsigned long *bp = phys_to_virt(base);
	struct intel_mp_floating *mpf;

	printk("scan SMP from %p for %ld bytes.\n",bp,length);

	if(sizeof(*mpf) != 16)
		printk("Error: MPF size\n");

	while(length > 0){
		mpf = (struct intel_mp_floating*)bp;
		
		
		if((*bp == SMP_MAGIC_IDENT) && (mpf->mpf_length ==1) && !mpf_checksum((unsigned char*)bp,16) && ((mpf->mpf_specification == 1) || (mpf->mpf_specification == 4))){
		
			smp_found_config = 1;	
			printk("found SMP MP-table at %08lx\n",virt_to_phys(mpf));
		
			reserve_bootmem(virt_to_phys(mpf),PAGE_SIZE);
			if(mpf->mpf_physptr)
				reserve_bootmem(mpf->mpf_physptr,PAGE_SIZE);

			mpf_found = mpf;
			return 1;	
		}

		bp += 4;
		length -= 16;
	}
	
	return 0;
}

void __init find_intel_smp(void)
{
	unsigned int address;
		
	if(smp_scan_config(0x0,0x400) || smp_scan_config(639*0x400,0x400) || smp_scan_config(0xF0000,0x10000))
		return;

	address = *(unsigned short*)phys_to_virt(0x40E);
	address <<= 4;

	smp_scan_config(address,0x1000);
	if(smp_found_config)
		printk("MP table in the EBDA can be UNSAFE\n");
	else
		printk("MP table not found,so badly\n");

}
void __init find_smp_config(void)
{

	find_intel_smp();
}
