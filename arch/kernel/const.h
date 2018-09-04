#ifndef __CONST_H_
#define __CONST_H_

#define _AT(T,X) (X)
#define __AC(X,Y) (X##Y)
#define _AC(X,Y) __AC(X,Y)
#define __user 
#define __aligned(x) __attribute__((aligned(x)))

#define __iomem
#endif
