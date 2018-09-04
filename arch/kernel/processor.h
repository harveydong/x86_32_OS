#ifndef __PROCESSOR_H_
#define __PROCESSOR_H_
#include "page_64_types.h"
#include "percpu.h"

typedef struct {
	unsigned long seg;
}mm_segment_t;

//DECLARE_PER_CPU(struct orig_ist,orig_ist);

union irq_stack_union{
	char irq_stack[IRQ_STACK_SIZE];
	struct {
		char gs_base[40];
		unsigned long stack_canary;
	};
};
#endif
