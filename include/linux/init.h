#ifndef __INIT_H_
#define __INIT_H_

#ifndef __ASSEMBLY__

#endif


#define __init __attribute__((__section__(".text.init")))
#endif
