#ifndef __LOW_MM_TYPES_H_
#define __LOW_MM_TYPES_H_

typedef unsigned long pgdval_t;
typedef unsigned long pmdval_t;

typedef struct {
	pgdval_t pgd;
}pgd_t;


typedef struct {
	pmdval_t pmd;
}pmd_t;

extern pgd_t init_level4_pgt[];
extern pmd_t level2_kernel_pgt[512];
#define swapper_pg_dir init_level4_pgt


#endif
