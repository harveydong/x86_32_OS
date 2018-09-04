#ifndef __IRQ_H_
#define __IRQ_H_
#include "types.h"
#include "desc_defs.h"

#define NUM_VECTORS 256
#define NR_VECTORS 	256
enum{
	GATE_INTERRUPT = 0xE,
	GATE_TRAP = 0xF,
	GATE_CALL = 0xC,
	GATE_TASK = 0x5,
};

struct gate_struct64{
	u16 offset_low;
	u16 segment;
	unsigned ist:3,zero0:5,type:5,dpl:2,p:1;
	u16 offset_middle;
	u16 offset_high;
	u32 zero1;

}__attribute__((packed));
#define PTR_LOW(x) ((unsigned long long)(x) & 0xFFFF)
#define PTR_MIDDLE(x) (((unsigned long long)(x) >> 16)&0xFFFF)
#define PTR_HIGH(x) ((unsigned long long)(x) >> 32)

typedef struct gate_struct64 gate_desc;



extern gate_desc idt_table[];

extern struct desc_ptr idt_descr;

#define NUM_EXCEPTION_VECTORS 32
extern const char early_idt_handlers[NUM_EXCEPTION_VECTORS][10];
#endif
