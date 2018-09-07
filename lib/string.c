#include <linux/types.h>

void *memset(void *s,int c,size_t count)
{
	char *xs = (char *)s;
	
	while(count--)
		*xs++ = c;

	return s;
}
void *memmove(void *dest,const void *src,size_t  count)
{
}
