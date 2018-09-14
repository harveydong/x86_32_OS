#ifndef __SWAPCTL_H_
#define __SWAPCTL_H_


typedef struct freepages_vl{
	unsigned int min;
	unsigned int low;
	unsigned int high;

}freepages_vl;

typedef freepages_vl freepages_t;

extern freepages_t freepages;

#endif
