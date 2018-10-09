#ifndef __LIST_H_
#define __LIST_H_

struct list_head{
	struct list_head *next,*prev;

};
#define LIST_HEAD_INIT(name) {.next = &(name),.prev = &(name)}
#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

#define INIT_LIST_HEAD(ptr) do{\
	(ptr)->next = (ptr);(ptr)->prev = (ptr);\
}while(0)

static inline int list_empty(struct list_head *head)
{
	return head->next == head;
}
#endif
