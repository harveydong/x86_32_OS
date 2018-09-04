#ifndef __PERSONALITY_H_
#define __PERSONALITY_H_
#include <asm/ptrace.h>

typedef void (*lcall7_func)(int,struct pt_regs*);
struct exec_domain{
	const char *name;
	lcall7_func handler;
	unsigned char pers_low,pers_high;
	unsigned long *signal_map;
	unsigned long *signal_invmap;	

};

extern struct exec_domain default_exec_domain;
#endif
