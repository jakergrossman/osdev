#ifndef __DENTON_SEM_H
#define __DENTON_SEM_H

#include <denton/list.h>

#include <stdatomic.h>

typedef struct sem {
	_Atomic(long) count;
	struct list_head waitlist;
} sem_t;

void sem_init(sem_t* sem, long count)
{
	*sem = (sem_t) {
		.count = count,
		.waitlist = LIST_HEAD_INIT(sem->waitlist),
	};
}

#endif
