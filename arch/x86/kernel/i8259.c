#include <linux/init.h>
#include <linux/spinlock.h>
#include <asm/io.h>
#include <linux/irq.h>
#include <linux/types.h>

spinlock_t i8259A_lock = SPIN_LOCK_UNLOCKED;

static unsigned int cached_irq_mask = 0xFFFF;


#define __byte(x,y) (((unsigned char*)&(y))[x])

#define cached_21 (__byte(0,cached_irq_mask))
#define cached_A1 (__byte(1,cached_irq_mask))

void disable_8259A_irq(unsigned int irq)
{

}

void mask_and_ack_8259A(unsigned int irq)
{

}

static void enable_8259A_irq(unsigned int irq)
{
	unsigned int mask = ~(1 << irq);
	unsigned long flags;

	spin_lock_irqsave(&i8259A_lock,flags);
	
	cached_irq_mask &= mask;
	
}
static unsigned int startup_8259A_irq(unsigned int irq)
{
	enable_8259A_irq(irq);
	return 0;
}
static struct hw_interrupt_type i8259A_irq_type = {

	.name = "XT-PIC",
	.startup = startup_8259A_irq,
	.shutdown = shutdown_8259A_irq,
	.enable = enable_8259A_irq,
	.disable = disable_8259A_irq,
	.ack = mask_and_ack_8259A,
	.end = end_8259A_irq,
	.set_affinity = NULL
};
void __init init_8259A(int auto_eoi)
{
	unsigned long flags;
	spin_lock_irqsave(&i8259A_lock,flags);

	outb(0xff,0x21);
	outb(0xff,0xA1);
	
	outb_p(0x11,0x20);
	outb_p(0x20+0,0x21);
	outb_p(0x04,0x21);
	
	if(auto_eoi)
		outb_p(0x03,0x21);
	else
		outb_p(0x01,0x21);

	outb_p(0x11,0xA0);
	outb_p(0x20+8,0xA1);
	outb_p(0x02,0xA1);
	outb_p(0x01,0xA1);
	
	if(auto_eoi)
		i8259A_irq_type.ack = disable_8259A_irq;
	else
		i8259A_irq_type.ack = mask_and_ack_8259A;
	
	udelay(100);
	outb(cached_21,0x21);
	outb(cached_A1,0xA1);
	
	spin_unlock_irqrestore(&i8259A_lock,flags);
}
void __init init_ISA_irqs(void)
{
	int i;
	
	init_8259A(0);
	
}

void __init init_IRQ(void)
{
	int i;
	
	init_ISA_irqs();
	
	for(i = 0; i < NR_IRQS;i++){
		int vector = FIRST_EXTERNAL_VECTOR + i;
		if(vector != SYSCALL_VECTOR)
			set_intr_gate(vector,interrupt[i]);
	}

	set_intr_gate(FIRST_DEVICE_VECTOR,interrupt[0]);
	set_intr_gate(RESCHEDULE_VECTOR,reschedule_interrupt);
	
	set_intr_gate(INVALIDATE_TLB_VECTOR,invalidate_interrupt);
	set_intr_gate(CALL_FUNCTION_VECTOR,call_function_interrupt);

	set_intr_gate(LOCAL_TIMER_VECTOR,apic_timer_interrupt);
	set_intr_gate(SPURIOUS_APIC_VECTOR,spurious_interrupt);
	set_intr_gate(ERROR_APIC_VECTOR,error_interrupt);
	
	outb_p(0x34,0x43);
	outb_p(LATCH&0xff,0x40);
	outb(LATCH>>8,0x40);
	
	setup_irq(2,&irq2);
	
	

}
