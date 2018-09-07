#ifndef __IOPORT_H_
#define __IOPORT_H_

struct resource {
	const char *name;
	unsigned long start,end;
	unsigned long flags;
	struct resource *parent,*sibling;

};

#define IORESOURCE_BUSY 0x80000000
#endif
