#ifndef __ATOMIC_H_
#define __ATOMIC_H_

typedef struct{
	volatile int counter;
}atomic_t;

#define ATOMIC_INIT(i) {(i)}

#endif
