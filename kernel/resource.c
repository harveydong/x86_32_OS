#include <linux/ioport.h>
#include <asm/io.h>
#include <asm/errno.h>
#include <linux/spinlock.h>
#include <linux/types.h>

static rwlock_t resource_lock = RW_LOCK_UNLOCKED;

struct resource iomem_resource = {
	.name = "PCI mem",
	.start = 0x00000000,
	.end = 0xFFFFFFFF,
	.flags = IORESOURCE_MEM
};

struct resource ioport_resource = {
	.name = "PCI IO",
	.start = 0x0000,
	.end = IO_SPACE_LIMIT,
	.flags = IORESOURCE_IO

};

static struct resource * __request_resource(struct resource*root, struct resource *new)
{

	unsigned long start = new->start;
	unsigned long end = new->end;

	struct resource *tmp,**p;
	
	if(end < start)
		return root;

	if(start < root->start)
		return root;

	if(end >  root->end)
		return root;

	p = &root->child;
	for(;;){
		tmp = *p;
		if(!tmp || tmp->start > end){
			new->sibling = tmp;
			*p = new;
			new->parent = root;
			return NULL;
		}

		p = &tmp->sibling;
		if(tmp->end < start)
			continue;
		return tmp;
	}
}
int request_resource(struct resource *root,struct resource *new)
{
	struct resource *conflict;
		
	write_lock(&resource_lock);
	conflict = __request_resource(root,new);
	write_unlock(&resource_lock);
	return conflict ? -EBUSY:0;

}
