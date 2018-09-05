#include <asm/boot.h>
#include <asm/e820.h>
#include "printf.h"

#define SMAP             0x534d4150
#define RAM              1
#define RESERVED         2
#define ACPI_RECLAIMABLE 3
#define ACPI_NVS         4
#define BAD_MEMORY       5


int e820_detect(struct boot_params *params)
{
	struct biosregs iregs, oregs;
	struct e820entry buf;
	struct e820entry *entries = params->e820;

	const int size = sizeof(params->e820) / sizeof(buf);
	int count = 0;

	initregs(&iregs);
	iregs.ax = 0xe820;
	iregs.cx = sizeof(buf);
	iregs.edx = SMAP;
	iregs.di = (unsigned long)&buf;

	do {
		intcall(0x15, &iregs, &oregs);
		
		iregs.ebx = oregs.ebx;

		if (oregs.eflags & X86_CF_FLAG)
			break;

		if (oregs.eax != SMAP) {
			count = 0;
			break;
		}

				
		//e820map.map[count++] = buf;

		entries[count++] = buf;
	} while (iregs.ebx && count < size);

	params->e820_entries = count;
	//printf("params:%x\n",params);
//	printf("len:%d\n",sizeof(struct boot_params));
//	printf("addr:0x%x\n",&params->e820_entries);
	//while(1);
	return count;
}

static const char *e820_type(unsigned int type)
{
	switch (type) {
	case RAM: return "RAM";
	case RESERVED: return "reserved";
	case ACPI_RECLAIMABLE: return "ACPI reclaimable memory";
	case ACPI_NVS: return "ACPI NVS";

	case BAD_MEMORY:
	default: return "unknown";
	}
}

void e820_show(const struct boot_params *params)
{
	const struct e820entry *entries = params->e820;
	int i;
//	const struct e820map e820_map = params->e820_map;
	int type;

	printf("Memory map (%u ranges):\n", params->e820_entries);
//	printf("Memory map (%u ranges):\n", e820_map.nr_map);

	for (i = 0; i < params->e820_entries; ++i) {
		type = entries[i].type;
		printf("region 0x%x-",entries[i].addr);
		printf("0x%x ",entries[i].addr + entries[i].size - 1);
		printf(" %s\n",e820_type(type));
	}
}
