#include <asm/apicdef.h>
#include <asm/mpspec.h>

int nr_ioapics;

struct mpc_config_ioapic mp_ioapics[MAX_IO_APICS];
struct mpc_config_intsrc mp_irqs[MAX_IRQ_SOURCES];
int mp_irq_entries;
