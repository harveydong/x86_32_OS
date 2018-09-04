#ifndef __IRQFLAGS_H_
#define __IRQFLAGS_H_

static inline void native_irq_disable(void)
{
	asm volatile("cli":::"memory");
}
static inline raw_local_irq_disable(void)
{
	native_irq_disable();
}
#define local_irq_disable()\
	do{raw_local_irq_disable();}while(0)

#endif

