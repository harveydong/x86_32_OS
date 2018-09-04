#ifndef __INIT_H_
#define __INIT_H_

#define __cold
#define notrace __attribute__((no_instrument_function))
#define __section(S) __attribute__((__section__(#S)))


#ifndef __always_inline
#define __always_inline inline
#endif


#define __init __section(.init.text) __cold notrace
#define __page_aligned_data __section(.data..page_aligned)
#endif
