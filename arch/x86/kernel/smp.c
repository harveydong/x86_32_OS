
#include <linux/threads.h>
#include <asm/pgalloc.h>

struct tlb_state cpu_tlbstate[NR_CPUS] = {[0 ... NR_CPUS-1] = {&init_mm,0}};
