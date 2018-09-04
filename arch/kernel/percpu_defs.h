#ifndef __PERCPU_DEFS_H_
#define __PERCPU_DEFS_H_
#include "percpu.h"

#define DECLARE_PER_CPU(type,name)	\
	DECLARE_PER_CPU_SECTION(type,name,"")
#endif
