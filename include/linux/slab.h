#ifndef __LINUX_SLAB_H_
#define __LINUX_SLAB_H_

#if defined(__KERNEL__)
#include <linux/list.h>
#include <linux/spinlock.h>

#define CACHE_NAMELEN 20

struct kmem_cache_s {
	struct list_head slabs;
	struct list_head *firstnotfull;
	unsigned int objsize;
	unsigned int flags;
	unsigned int num;
	spinlock_t spinlock;	
	
	size_t colour;
	unsigned int colour_off;
	unsigned int colour_next;

	char name[CACHE_NAMELEN];
	struct list_head next;

	
};

typedef struct kmem_cache_s kmem_cache_t;

#define CFLGS_OFF_SLAB 0x010000UL

#define SLAB_NO_REAP 0x00001000UL

#define SLAB_LIMIT 0xFFFFFFFE
typedef unsigned int kmem_bufctl_t;

typedef struct slab_s {
	struct list_head list;
	unsigned long colouroff;
	void *s_mem;
	unsigned int inuse;
	kmem_bufctl_t free;
}slab_t;

extern void kmem_cache_init(void);

#endif
#endif
