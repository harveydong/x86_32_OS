#ifndef __PERCPU_H_
#define __PERCPU_H_
#include "page_64_types.h"
#include "const.h"


#define PER_CPU_ATTRIBUTES
#define PER_CPU_DEF_ATTRIBUTES


#define __percpu
#define PER_CPU_BASE_SECTION ".data..percpu"
#define PER_CPU_FIRST_SECTION "..first"
#define INIT_PER_CPU_VAR(var) init_per_cpu__##var

#define __PCPU_ATTRS(sec) \
	__percpu __attribute__((section(PER_CPU_BASE_SECTION sec))) \
	PER_CPU_ATTRIBUTES
#define DECLARE_PER_CPU_SECTION(type,name,sec)\
	extern __PCPU_ATTRS(sec) PER_CPU_DEF_ATTRIBUTES\
	__typeof__(type) name

#define DECLARE_PER_CPU_PAGE_ALIGNED(type,name)\
	DECLARE_PER_CPU_SECTION(type,name,"..page_aligned")\
	__aligned(PAGE_SIZE)


#define DECLARE_PER_CPU_FIRST(type,name)\
	DECLARE_PER_CPU_SECTION(type,name,PER_CPU_FIRST_SECTION)


#define DECLARE_INIT_PER_CPU(var)\
	extern typeof(var) init_per_cpu_var(var)


#define DEFINE_PER_CPU_SECTION(type,name,sec)\
	__PCPU_ATTRS(sec) PER_CPU_DEF_ATTRIBUTES\
	__typeof__(type) name
#define DEFINE_PER_CPU_FIRST(type,name)\
	DEFINE_PER_CPU_SECTION(type,name,PER_CPU_FIRST_SECTION)

#define DEFINE_PER_CPU_PAGE_ALIGNED(type,name) \
	DEFINE_PER_CPU_SECTION(type,name,"..page_aligned") \
	__aligned(PAGE_SIZE)

#define DEFINE_PER_CPU(type,name)	\
	DEFINE_PER_CPU_SECTION(type,name,"")
#endif
