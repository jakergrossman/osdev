#ifndef __DENTON_SEM_H
#define __DENTON_SEM_H

#include <asm/sync/spinlock.h>
#include <denton/list.h>

#include <stdbool.h>
#include <stdatomic.h>

typedef struct sem {
	spinlock_t lock;
	long count;
	struct list_head waitlist;
} sem_t;

#define __SEM_INIT(name, cnt) { \
	.lock = SPINLOCK_INIT(0), \
	.count = (cnt), \
	.waitlist = LIST_HEAD_INIT((name).waitlist) \
}

#define SEM_DECL(name, n) \
	struct sem name = __SEM_INIT(name, n);


void sem_init(sem_t* sem, long count);
void sem_up(sem_t* sem);
int  sem_down(sem_t* sem);
bool sem_trydown(sem_t* sem);

#endif
