#ifndef __DELAY_H_
#define __DELAY_H_

extern void __udelay(unsigned long usecs);
extern void __delay(unsigned long loops);
#define udelay(n) __udelay(n)

#endif
