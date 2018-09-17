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
	
	return dest;
}

void *memcpy(void *dst,const void *src,size_t count)
{
	char *tmp = (char*)dst,*s = (char*)src;

	while(count--)
		*tmp++ = *s++;

	return dst;
}

int memcmp(const void *cs,const void *ct,size_t count)
{
	const unsigned char *sul,*su2;
	signed char res = 0;

	for(sul = cs,su2 = ct;0 < count; ++sul,++su2,count--)
		if((res = *sul - *su2) != 0)
			break;

	return res;
}
