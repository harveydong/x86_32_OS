#ifndef __STDARG_H_
#define __STDARG_H_

typedef char *va_list;

#define _AUPBND (sizeof(int) - 1)
#define _ADNBND (sizeof(int) - 1)


#define _bnd(X,bnd) (((sizeof(X)) + (bnd)) & (~(bnd)))

#define va_arg(ap,T) (*(T*)(((ap) += (_bnd(T,_AUPBND))) - (_bnd(T,_ADNBND))))
#define va_end(ap) (void)0
#define va_start(ap,A) (void)((ap) = (((char*)&(A)) + (_bnd(A,_AUPBND))))
#endif
