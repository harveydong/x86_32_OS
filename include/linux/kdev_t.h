#ifndef __KDEV_T_H_
#define __KDEV_T_H_

typedef unsigned short kdev_t;

#define MAJOR(dev) ((dev) >> 8)
#define MINOR(dev) ((dev) & 0xff)
#define MKDEV(ma,mi) ((ma) << 8|(mi))
static inline kdev_t to_kdev_t(int dev)
{
	int major,minor;
	
	major = (dev >> 8);
	minor = (dev & 0xff);
	
	return MKDEV(major,minor);

}
#endif
