#include "mm_types.h"

struct mm_struct init_mm = {
	.pgd = swapper_pg_dir,

};
