#ifndef __LIST_H_
#define __LIST_H_

struct list_head{
	struct list_head *next,*prev;

};
#define LIST_HEAD_INIT(name) {&(name),&(name)}


#endif
