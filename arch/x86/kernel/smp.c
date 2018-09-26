
#include <linux/threads.h>
#include <asm/pgalloc.h>
#include <linux/linkage.h>

struct tlb_state cpu_tlbstate[NR_CPUS] = {[0 ... NR_CPUS-1] = {&init_mm,0}};

asmlinkage void smp_reschedule_interrupt(void)
{
	//ack_APIC_irq();
}

asmlinkage void smp_call_function_interrupt(void)
{

}


asmlinkage void smp_invalidate_interrupt(void)
{

}
