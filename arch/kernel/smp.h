#ifndef __SMP_H_
#define __SMP_H_
#include "percpu_fun.h"
#include "percpu_defs.h"

DECLARE_PER_CPU(int,cpu_number);

#define raw_smp_processor_id() (percpu_read(cpu_number))

#define smp_processor_id() raw_smp_processor_id()
#endif
