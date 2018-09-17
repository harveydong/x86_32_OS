#ifndef __APICDEF_H_
#define __APICDEF_H_
//#include <asm/fixmap.h>


#define MAX_IO_APICS 8



#define APIC_ID 0x20
#define GET_APIC_ID(x) (((x) >> 24) & 0x0F)

#endif
