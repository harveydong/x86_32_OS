#include <linux/init.h>
#include <linux/spinlock.h>
#include <asm/io.h>
#include <asm/boot.h>

#include <linux/irq.h>
#include <linux/types.h>
#include <asm/delay.h>
#include <asm/irq.h>
#include <asm/hw_irq.h>
#include <linux/timex.h>
#include <asm/desc.h>


BUILD_COMMON_IRQ()

#define BI(x,y)\
	BUILD_IRQ(x##y)

#define BUILD_16_IRQS(x)\
	BI(x,0) BI(x,1)	BI(x,2) BI(x,3) \
	BI(x,4) BI(x,5)	BI(x,6) BI(x,7) \
	BI(x,8) BI(x,9)	BI(x,a) BI(x,b) \
	BI(x,c) BI(x,d)	BI(x,e) BI(x,f) 

BUILD_16_IRQS(0x0)
BUILD_16_IRQS(0x1) BUILD_16_IRQS(0x2) BUILD_16_IRQS(0x3) BUILD_16_IRQS(0x4) 
BUILD_16_IRQS(0x5) BUILD_16_IRQS(0x6) BUILD_16_IRQS(0x7) BUILD_16_IRQS(0x8) 
BUILD_16_IRQS(0x9) BUILD_16_IRQS(0xa) BUILD_16_IRQS(0xb) BUILD_16_IRQS(0xc) BUILD_16_IRQS(0xd)  

#undef BUILD_16_IRQS
#undef BI



BUILD_SMP_INTERRUPT(reschedule_interrupt,RESCHEDULE_VECTOR)
BUILD_SMP_INTERRUPT(invalidate_interrupt,INVALIDATE_TLB_VECTOR)
BUILD_SMP_INTERRUPT(call_function_interrupt,CALL_FUNCTION_VECTOR)
BUILD_SMP_INTERRUPT(error_interrupt,ERROR_APIC_VECTOR)
BUILD_SMP_INTERRUPT(spurious_interrupt,SPURIOUS_APIC_VECTOR)
BUILD_SMP_TIMER_INTERRUPT(apic_timer_interrupt,LOCAL_TIMER_VECTOR)



#define IRQD(x,y) IRQ##x##y##_interrupt


#define IRQLIST_16(x) \
	IRQD(x,0), IRQD(x,1),IRQD(x,2),IRQD(x,3),\
	IRQD(x,4), IRQD(x,5),IRQD(x,6),IRQD(x,7), \
	IRQD(x,8), IRQD(x,9),IRQD(x,a),IRQD(x,b), \
	IRQD(x,c), IRQD(x,d),IRQD(x,e),IRQD(x,f)





void (asmlinkage *interrupt[NR_IRQS])(void) = {
	IRQLIST_16(0x0),
	IRQLIST_16(0x1),
	IRQLIST_16(0x2),
	IRQLIST_16(0x3),
	IRQLIST_16(0x4),
	IRQLIST_16(0x5),
	IRQLIST_16(0x6),
	IRQLIST_16(0x7),
	IRQLIST_16(0x8),
	IRQLIST_16(0x9),
	IRQLIST_16(0xa),
	IRQLIST_16(0xb),
	IRQLIST_16(0xc),
	IRQLIST_16(0xd)
};


#undef IRQD
#undef IRQLIST_16


spinlock_t i8259A_lock = SPIN_LOCK_UNLOCKED;

static unsigned int cached_irq_mask = 0xFFFF;

#define __byte(x,y) (((unsigned char*)&(y))[x])

#define cached_21 (__byte(0,cached_irq_mask))
#define cached_A1 (__byte(1,cached_irq_mask))

void disable_8259A_irq(unsigned int irq)
{
	unsigned int mask = 1 << irq;
	unsigned long flags;

	spin_lock_irqsave(&i8259A_lock,flags);
	cached_irq_mask |= mask;

	if(irq & 8)
		outb(cached_A1,0xA1);
	else
		outb(cached_21,0x21);

	spin_unlock_irqrestore(&i8259A_lock,flags);		

}

static inline int i8259A_irq_real(unsigned int irq)
{
	int value;
	int irqmask = 1 << irq;

	if(irq < 8){
		outb(0x0B,0x20);
		value = inb(0x20) & irqmask;
		outb(0x0A,0x20);	
		return value;
	}

	outb(0x0B,0xA0);
	value = inb(0xA0) & (irqmask >> 8);
	outb(0x0A,0xA0);
	return value;
}

void mask_and_ack_8259A(unsigned int irq)
{
	unsigned int irqmask = 1 << irq;
	unsigned long flags;
	
	spin_lock_irqsave(&i8259A_lock,flags);
	if(cached_irq_mask &irqmask)
		goto spurious_8259A_irq;

	cached_irq_mask |= irqmask;

handle_real_irq:
	if(irq &8){
		inb(0xA1);
		outb(cached_A1,0xA1);
		outb(0x60 + (irq&7),0xA0);
		outb(0x62,0x20);
	}else{
		inb(0x21);
		outb(cached_21,0x21);
		outb(0x60+irq,0x20);
	}
	spin_unlock_irqrestore(&i8259A_lock,flags);
	return;

spurious_8259A_irq:
	
	if(i8259A_irq_real(irq))
		goto handle_real_irq;

	{
		static int spurious_irq_mask;
		if(!(spurious_irq_mask & irqmask)){
			printk("spurious 8259A interrrupt\n");
			spurious_irq_mask |= irqmask;
		}
		
		irq_err_count++;
	
		goto handle_real_irq;
	}


}
static void enable_8259A_irq(unsigned int irq)
{
	unsigned int mask = ~(1 << irq);
	unsigned long flags;

	spin_lock_irqsave(&i8259A_lock,flags);
	
	cached_irq_mask &= mask;
	if(irq & 8)
		outb(cached_A1,0xA1);
	else
		outb(cached_21,0x21);

	spin_unlock_irqrestore(&i8259A_lock,flags);
	
}



static void end_8259A_irq(unsigned int irq)
{
	if(!(irq_desc[irq].status & (IRQ_DISABLED|IRQ_INPROGRESS))){
		enable_8259A_irq(irq);
	}
}


static unsigned int startup_8259A_irq(unsigned int irq)
{
	enable_8259A_irq(irq);
	return 0;
}


#define shutdown_8259A_irq disable_8259A_irq


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

	outb(0xff,0x21); //for OCW1 IMR.
	outb(0xff,0xA1);//for OCW1 IMR.
	
	outb_p(0x11,0x20);//ICW1
	outb_p(0x20+0,0x21);//ICW2
	outb_p(0x04,0x21);//ICW3
	
	if(auto_eoi)
		outb_p(0x03,0x21);//ICW4
	else
		outb_p(0x01,0x21);

	outb_p(0x11,0xA0);//SLAVE ICW1
	outb_p(0x20+8,0xA1);//SLAVE ICW2
	outb_p(0x02,0xA1);//SLAVE ICW3
	outb_p(0x01,0xA1);//SLAVE ICW4
	
	if(auto_eoi)
		i8259A_irq_type.ack = disable_8259A_irq;
	else
		i8259A_irq_type.ack = mask_and_ack_8259A;
	
	udelay(100);
	outb(cached_21,0x21);//MASTER OCW1,IMR=0XFF
	outb(cached_A1,0xA1);//slave OCW1,IMR=0XFF
	
	spin_unlock_irqrestore(&i8259A_lock,flags);
}
void __init init_ISA_irqs(void)
{
	int i;
	
	init_8259A(0);

	for(i = 0; i < NR_IRQS;i++){
		
		irq_desc[i].status = IRQ_DISABLED;
		irq_desc[i].action = 0;
		irq_desc[i].depth = 1;
	
		if(i < 16){
			irq_desc[i].handler = &i8259A_irq_type;
		}else{
			irq_desc[i].handler = &no_irq_type;
		}

	}	
}

void __init init_IRQ(void)
{
	int i;
	
	init_ISA_irqs();
	
	for(i = 0; i < NR_IRQS;i++){
		int vector = FIRST_EXTERNAL_VECTOR + i;
		if(vector != SYSCALL_VECTOR){
			set_intr_gate(vector,interrupt[i]);
		}	
	}

	set_intr_gate(FIRST_DEVICE_VECTOR,interrupt[0]);
	set_intr_gate(RESCHEDULE_VECTOR,reschedule_interrupt);
	
	set_intr_gate(INVALIDATE_TLB_VECTOR,invalidate_interrupt);
	set_intr_gate(CALL_FUNCTION_VECTOR,call_function_interrupt);

	set_intr_gate(LOCAL_TIMER_VECTOR,apic_timer_interrupt);
	set_intr_gate(SPURIOUS_APIC_VECTOR,spurious_interrupt);
	set_intr_gate(ERROR_APIC_VECTOR,error_interrupt);

	__asm__ __volatile__("lidt %0":"=m"(idt_descr));

/*for 8253 pit 
* control port=0x43
* counter0 port = 0x40
* counter1 port = 0x41
* counter2 port = 0x42
*/	

/*set freq*/
	outb_p(0x36,0x43);//set 100Hz

/*set counter*/
	outb_p(LATCH&0xff,0x40);
	outb(LATCH>>8,0x40);
	
//	setup_irq(2,&irq2);
	

	

}
