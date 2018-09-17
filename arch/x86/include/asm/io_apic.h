#ifndef __IO_APIC_H_
#define __IO_APIC_H_
#include <asm/mpspec.h>
#include <asm/apicdef.h>

extern struct mpc_config_ioapic mp_ioapics[MAX_IO_APICS];
extern int nr_ioapics;

#endif
