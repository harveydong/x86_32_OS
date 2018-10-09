#include <linux/init.h>
#include <linux/types.h>
#include <asm/semaphore.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/cache.h>
#include <asm/page.h>

static struct semaphore cache_chain_sem;

static kmem_cache_t cache_cache = {
	.slabs = LIST_HEAD_INIT(cache_cache.slabs),
	.firstnotfull = &cache_cache.slabs,
	.objsize = sizeof(kmem_cache_t),
	.flags = SLAB_NO_REAP,
	.spinlock = SPIN_LOCK_UNLOCKED,
	.colour_off = L1_CACHE_BYTES,
	.name = "kmem_cache"
};

#define cache_chain (cache_cache.next)

static void kmem_cache_estimate(unsigned long gfporder,size_t size,int flags,size_t *left_over,unsigned int *num)
{
	int i;
	size_t wastage = PAGE_SIZE << gfporder;

	size_t extra = 0;
	size_t base = 0;

	if(!(flags & CFLGS_OFF_SLAB)){
		base = sizeof(slab_t);
		extra = sizeof(kmem_bufctl_t);
	}

	i = 0;
	while(i * size + L1_CACHE_ALIGN(base + i*extra) <= wastage)
		i++;

	if(i > 0)
		i--;

	if(i > SLAB_LIMIT)
		i = SLAB_LIMIT;

	*num = i;
	wastage -= i*size;
	wastage -= L1_CACHE_ALIGN(base + i*extra);
	*left_over = wastage;
}

void __init kmem_cache_init(void)
{

	size_t left_over;

	init_MUTEX(&cache_chain_sem);
	
	INIT_LIST_HEAD(&cache_chain);

	kmem_cache_estimate(0,cache_cache.objsize,0,&left_over,&cache_cache.num);

	if(!cache_cache.num)
	{
		printk("Error cache_cache num!\n");
		while(1);
	}

	cache_cache.colour = left_over / cache_cache.colour_off;
	cache_cache.colour_next = 0;

	printk("kmem cache init done!\n");
}


