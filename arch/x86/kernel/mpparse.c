#include <linux/init.h>
#include <asm/mpspec.h>
#include <asm/page.h>
#include <linux/bootmem.h>
#include <asm/io.h>
#include <linux/string.h>
#include <asm/io_apic.h>
#include <asm/apicdef.h>
#include <asm/fixmap.h>


int smp_found_config;
static struct intel_mp_floating *mpf_found;
int pic_mode;
unsigned long mp_lapic_addr;

unsigned int boot_cpu_id = -1UL;
static unsigned int num_processors;

unsigned long phys_cpu_present_map;

int apic_version[MAX_APICS];
int mp_bus_id_to_type[MAX_MP_BUSSES];
int mp_bus_id_to_pci_bus[MAX_MP_BUSSES] = {-1,};
int mp_current_pci_id;


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


static char __init *mpc_family(int family,int model)
{
	static char n[32];
	static char *model_defs[] = {
		"80486DX","80486DX",
		"80486SX","80486DX/2 or 80487",
		"80486SL","80486SX/2",
		"Unknown","80486DX/2-WB"
		"ZX-CHX001","ZX-CHX002"
	};

	switch(family){
		case 0x04:
			if(model < 10)
				return model_defs[model];
			break;

		case 0x05:
			return ("Pentium(tm)");
			
		case 0x06:
			return ("Pentium(tm) Pro");
		

	}

	printk("Unknow CPU [%d:%d]",family,model);

	return 1;
}

static void __init MP_processor_info(struct mpc_config_processor *m)
{
	int ver;

	if(!(m->mpc_cpuflag &CPU_ENABLED)){
		return;
	}

	printk("Processor #%d %s APIC version %d\n",m->mpc_apicid,mpc_family((m->mpc_cpufeature & CPU_FAMILY_MASK) >> 8,(m->mpc_cpufeature &CPU_MODEL_MASK) >> 4),m->mpc_apicver);

	if (m->mpc_featureflag&(1<<0))
		printk("    Floating point unit present.\n");
	if (m->mpc_featureflag&(1<<7))
		printk("    Machine Exception supported.\n");
	if (m->mpc_featureflag&(1<<8))
		printk("    64 bit compare & exchange supported.\n");
	if (m->mpc_featureflag&(1<<9))
		printk("    Internal APIC present.\n");
	if (m->mpc_featureflag&(1<<11))
		printk("    SEP present.\n");
	if (m->mpc_featureflag&(1<<12))
		printk("    MTRR  present.\n");
	if (m->mpc_featureflag&(1<<13))
		printk("    PGE  present.\n");
	if (m->mpc_featureflag&(1<<14))
		printk("    MCA  present.\n");
	if (m->mpc_featureflag&(1<<15))
		printk("    CMOV  present.\n");
	if (m->mpc_featureflag&(1<<16))
		printk("    PAT  present.\n");
	if (m->mpc_featureflag&(1<<17))
		printk("    PSE  present.\n");
	if (m->mpc_featureflag&(1<<18))
		printk("    PSN  present.\n");
	if (m->mpc_featureflag&(1<<19))
		printk("    Cache Line Flush Instruction present.\n");
	/* 20 Reserved */
	if (m->mpc_featureflag&(1<<21))
		printk("    Debug Trace and EMON Store present.\n");
	if (m->mpc_featureflag&(1<<22))
		printk("    ACPI Thermal Throttle Registers  present.\n");
	if (m->mpc_featureflag&(1<<23))
		printk("    MMX  present.\n");
	if (m->mpc_featureflag&(1<<24))
		printk("    FXSR  present.\n");
	if (m->mpc_featureflag&(1<<25))
		printk("    XMM  present.\n");
	if (m->mpc_featureflag&(1<<26))
		printk("    Willamette New Instructions  present.\n");
	if (m->mpc_featureflag&(1<<27))
		printk("    Self Snoop  present.\n");
	/* 28 Reserved */
	if (m->mpc_featureflag&(1<<29))
		printk("    Thermal Monitor present.\n");
	/* 30, 31 Reserved */


	if (m->mpc_cpuflag & CPU_BOOTPROCESSOR) {
		printk("    Bootup CPU\n");
		boot_cpu_id = m->mpc_apicid;
	}
	num_processors++;	


	if(m->mpc_apicid > MAX_APICS){
		printk("Processor #%d INVALID,(MAX ID:%d)\n",m->mpc_apicid,MAX_APICS);
		return;
	}

	ver = m->mpc_apicver;
	
	phys_cpu_present_map |= 1 << m->mpc_apicid;
	
	if(ver == 0x0)
	{
		printk("BIOS bug,APIC version is 0 for CPU #%d!,fixing up to 0x10\n",m->mpc_apicid);
		ver = 0x10;
	}

	apic_version[m->mpc_apicid] = ver;
}


static void __init MP_ioapic_info(struct mpc_config_ioapic*m)
{
	if((!m->mpc_flags & MPC_APIC_USABLE))
		return;

	printk("IO-APIC #%d Version %d at 0x%lx.\n",m->mpc_apicid,m->mpc_apicver,m->mpc_apicaddr);

	if(nr_ioapics >= MAX_IO_APICS){
		printk("IO apic num exceeded\n");
		BUG();
	}

	mp_ioapics[nr_ioapics] = *m;

	nr_ioapics++;
}



static void __init MP_bus_info(struct mpc_config_bus*m)
{
	char str[7];

	memcpy(str,m->mpc_bustype,6);

	str[6] = 0;
	printk("Bus #%d is %s\n",m->mpc_busid,str);

	if(strncmp(str,BUSTYPE_ISA,sizeof(BUSTYPE_ISA)))
		mp_bus_id_to_type[m->mpc_busid] = MP_BUS_ISA;
	else if(strncmp(str,BUSTYPE_EISA,sizeof(BUSTYPE_EISA)))
		mp_bus_id_to_type[m->mpc_busid] = MP_BUS_EISA;
	else if(strncmp(str,BUSTYPE_PCI,sizeof(BUSTYPE_PCI))){
		mp_bus_id_to_type[m->mpc_busid] = MP_BUS_PCI;
		mp_bus_id_to_pci_bus[m->mpc_busid] = mp_current_pci_id;
		mp_current_pci_id++;
	}else{
		printk("Unknown bustype %s--ignoring\n",str);
	}

}

static void __init MP_intsrc_info(struct mpc_config_intsrc *m)
{
	mp_irqs[mp_irq_entries] = *m;

	printk("Int: type %d,pol %d,trig %d,bus %d,IRQ %x,APIC ID %x,APIC INT %x\n",m->mpc_irqtype,m->mpc_irqflag & 3,(m->mpc_irqflag >> 2)&3,m->mpc_srcbus,m->mpc_srcbusirq,m->mpc_dstapic,m->mpc_dstirq);

	if(++mp_irq_entries == MAX_IRQ_SOURCES){
		printk("MAX # of irq sources exceeded!!\n");
		while(1);
	}
}

static int __init smp_read_mpc(struct mp_config_table *mpc)
{
	char str[16];
	int count = sizeof(*mpc);
	
	unsigned char *mpt = ((unsigned char*)mpc)+count;

	if(memcmp(mpc->mpc_signature,MPC_SIGNATURE,4)){
		printk("SMP mptable bad\n");
		return 1;
	}

	if(mpf_checksum((unsigned char*)mpc,mpc->mpc_length)){
		printk("SMP mptable:checksum error\n");
		return 1;
	}

	if(mpc->mpc_spec != 0x01 && mpc->mpc_spec != 0x04){
		printk("Bad Config Table Version\n");
		return 1;
	}

	memcpy(str,mpc->mpc_oem,8);
	str[8] = 0;
	printk("OEM ID:%s ",str);
	
	memcpy(str,mpc->mpc_productid,12);
	str[12] = 0;
	printk("Product ID:%s ",str);

	printk("APIC at:0x%lx\n",mpc->mpc_lapic);
	
	mp_lapic_addr = mpc->mpc_lapic;

	while(count < mpc->mpc_length){
		switch(*mpt){
			case MP_PROCESSOR:
			{
				struct mpc_config_processor *m = (struct mpc_config_processor*)mpt;
					
				MP_processor_info(m);
				mpt += sizeof(*m);
				count += sizeof(*m);
				break;
			}
	
			case MP_IOAPIC:
			{
				struct mpc_config_ioapic *m = (struct mpc_config_ioapic*)mpt;
				MP_ioapic_info(m);
				mpt += sizeof(*m);	
				count += sizeof(*m);
				break;	
	
			}
		
			case MP_INTSRC:
			{
				struct mpc_config_intsrc *m = (struct mpc_config_intsrc*)mpt;
				MP_intsrc_info(m);

				printk("This is MP INTSRC\n");
				mpt += sizeof(*m);
				count += sizeof(*m);
				break;
			}

			case MP_LINTSRC:
			{
				struct mpc_config_lintsrc *m = (struct mpc_config_lintsrc*)mpt;
				printk("this is MP LINTSRC\n");
				mpt += sizeof(*m);
				count += sizeof(*m);
				break;
			}
			
			case MP_BUS:
			{
				struct mpc_config_bus *m = (struct mpc_config_bus*)mpt;
				MP_bus_info(m);
				printk("this is MP bus\n");
				mpt += sizeof(*m);
				count += sizeof(*m);
				break;
			}
		}
	}

	return num_processors;
	
}


void __init get_smp_config(void)
{
	struct intel_mp_floating *mpf = mpf_found;

	printk("MultiProcessor Specification V1.%d\n",mpf->mpf_specification);

	if(mpf->mpf_feature2 &(1<<7)){
		printk("IMCR and PIC compatibility mode.\n");
		pic_mode =1;
	}else{
		printk("Virutal Wire compatibility mode.\n");
		pic_mode = 0;
	}

	if(mpf->mpf_feature1 != 0){
		printk("Default MP configuration #%d\n",mpf->mpf_feature1);
	}else if(mpf->mpf_physptr){
		printk("mpf physptr is NOT null\n");
	
		smp_read_mpc((void*)mpf->mpf_physptr);
	}
	
	printk("Processors number: %d\n",num_processors);
}
