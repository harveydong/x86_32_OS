#ifndef __P_TYPES_H_
#define __P_TYPES_H_
#include "page_64_types.h"
#include "sched.h"
#include "mm_low_types.h"
#include "mm_types.h"



static inline pgd_t native_make_pgd(pgdval_t val)
{
	return (pgd_t){val};
}


static inline void native_set_pgd(pgd_t *pgdp,pgd_t pgd)
{
	*pgdp = pgd;
}

static inline void native_pgd_clear(pgd_t *pgd)
{
	native_set_pgd(pgd,native_make_pgd(0));
}



static inline pmdval_t native_pmd_val(pmd_t pmd)
{
	return pmd.pmd;
}
static inline int pmd_none(pmd_t pmd)
{
	return (unsigned long)native_pmd_val(pmd) == 0;
}


static inline pmd_t native_make_pmd(pmdval_t val)
{
	return (pmd_t){ val };
}

static inline void native_set_pmd(pmd_t *pmdp,pmd_t pmd)
{
	*pmdp = pmd;
}
extern void cleanup_highmap(void);


#define pgd_clear(pgd) native_pgd_clear(pgd)

#define pgd_index(address) (((address) >> PGDIR_SHIFT)&(PTRS_PER_PGD - 1))
#define pgd_offset(mm,address) ((mm)->pgd + pgd_index(address))
#define pgd_offset_k(address) pgd_offset(&init_mm,(address))


#define set_pmd(pmdp,pmd) native_set_pmd(pmdp,pmd)

#define __pmd(x) native_make_pmd(x)
#endif
