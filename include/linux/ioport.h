#ifndef __IOPORT_H_
#define __IOPORT_H_

struct resource {
	const char *name;
	unsigned long start,end;
	unsigned long flags;
	struct resource *parent,*sibling,*child;

};

#define IORESOURCE_MEM 0x00000200
#define IORESOURCE_IO 0x00000100
#define IORESOURCE_IRQ 0x00000400
#define IORESOURCE_DMA 0x00000800

#define IORESOURCE_BUSY 0x80000000

extern struct resource ioport_resource;
extern struct resource iomem_resource;

extern int request_resource(struct resource*root,struct resource*new);

#endif
