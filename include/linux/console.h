#ifndef __CONSOLE_H_
#define __CONSOLE_H_

struct console{
	char name[16];
	void (*write)(struct console*,const char *,unsigned);
	short flags;
	short index;

};

#endif
