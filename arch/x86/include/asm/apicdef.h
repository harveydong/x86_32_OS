#ifndef __APICDEF_H_
#define __APICDEF_H_
//#include <asm/fixmap.h>


#define MAX_IO_APICS 8



#define APIC_ID 0x20
#define APIC_EOI 0xB0
#define APIC_LVTT 0x320
#define APIC_TDCR 0x3E0
#define APIC_TMICT 0x380
#define APIC_ISR 0x100
#define APIC_ESR 0x280

#define GET_APIC_ID(x) (((x) >> 24) & 0x0F)


#define SET_APIC_TIMER_BASE(x) (((x) << 18))
#define APIC_TIMER_BASE_DIV 0x2
#define APIC_LVT_TIMER_PERIODIC (1<<17)
#define APIC_TDR_DIV_1 0xB
#define APIC_TDR_DIV_16 0x3

#define APIC_TDR_DIV_TMBASE (1<<2)

#endif
