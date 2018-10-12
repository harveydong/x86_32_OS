#ifndef __FS_H_
#define __FS_H_
#include <linux/kdev_t.h>
#include <asm/atomic.h>
#include <linux/list.h>

extern kdev_t ROOT_DEV;

struct buffer_head {
	atomic_t b_count;

};

struct address_space {
	struct list_head clean_pages;
};
#endif
