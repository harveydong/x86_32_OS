#ifndef __MPSPEC_H_
#define __MPSPEC_H_

#define SMP_MAGIC_IDENT (('_' << 24) | ('P' << 16) | ('M' << 8)|'_')

#define MAX_MP_BUSSES 32
#define MAX_IRQ_SOURCES 128


#define MP_PROCESSOR 0
#define MP_BUS 1
#define MP_IOAPIC 2
#define MP_INTSRC 3
#define MP_LINTSRC 4

#define MAX_APICS 16


#define BUSTYPE_EISA "EISA"
#define BUSTYPE_ISA "ISA"
#define BUSTYPE_PCI "PCI"


enum mp_bustype{
	MP_BUS_ISA = 1,
	MP_BUS_EISA,
	MP_BUS_PCI,
	MP_BUS_MCA
};


struct mpc_config_processor{
	unsigned char mpc_type;	
	unsigned char mpc_apicid;
	unsigned char mpc_apicver;
	unsigned char mpc_cpuflag;
	
#define CPU_ENABLED 1
#define CPU_BOOTPROCESSOR 2
	unsigned long mpc_cpufeature;
	
#define CPU_STEPPING_MASK 0x0F
#define CPU_MODEL_MASK 0xF0
#define CPU_FAMILY_MASK 0xF00
	unsigned long mpc_featureflag;

	unsigned long mpc_reserved[2];
};


struct intel_mp_floating{

	char mpf_signature[4];/*_MP_*/
	unsigned int mpf_physptr;
	unsigned char mpf_length;
	unsigned char mpf_specification;
	unsigned char mpf_checksum;
	unsigned char mpf_feature1;
	unsigned char mpf_feature2;
	unsigned char mpf_feature3;
	unsigned char mpf_feature4;
	unsigned char mpf_feature5;

};

struct mp_config_table{

	char mpc_signature[4];
#define MPC_SIGNATURE "PCMP"
	unsigned short mpc_length;
	char mpc_spec;
	char mpc_checksum;
	char mpc_oem[8];
	char mpc_productid[12];
	unsigned long mpc_oemptr;
	unsigned short mpc_oemsize;
	unsigned short mpc_oemcount;
	unsigned long mpc_lapic;
	unsigned long reserved;
};


struct mpc_config_bus{

	unsigned char mpc_type;
	unsigned char mpc_busid;
	unsigned char mpc_bustype[6] __attribute((packed));
};

struct mpc_config_ioapic{
	unsigned char mpc_type;
	unsigned char mpc_apicid;
	unsigned char mpc_apicver;
	unsigned char mpc_flags;
#define MPC_APIC_USABLE 0x01
	unsigned long mpc_apicaddr;

};


struct mpc_config_intsrc
{
	unsigned char mpc_type;
	unsigned char mpc_irqtype;
	unsigned short mpc_irqflag;
	unsigned char mpc_srcbus;
	unsigned char mpc_srcbusirq;
	unsigned char mpc_dstapic;
	unsigned char mpc_dstirq;
};

struct mpc_config_lintsrc
{
	unsigned char mpc_type;
	unsigned char mpc_irqtype;
	unsigned short mpc_irqflag;
	unsigned char mpc_srcbusid;
	unsigned char mpc_srcbusirq;
	unsigned char mpc_destapic;	
#define MP_APIC_ALL	0xFF
	unsigned char mpc_destapiclint;
};


extern void find_smp_config(void);

extern unsigned int boot_cpu_id;



extern int smp_found_config;
extern void get_smp_config(void);

extern unsigned long mp_lapic_addr;

#endif
