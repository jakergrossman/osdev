#ifndef __DENTON_TIME_TIMER_H
#define __DENTON_TIME_TIMER_H

#include <denton/list.h>

enum timer_mode {
	TIMER_CONTINUE,
	TIMER_ONESHOT,
};

struct timer;

typedef void (*timer_cb)(struct timer * timer);

struct timer {
	enum timer_mode mode;
	unsigned long expires;
	unsigned long period;
	timer_cb expire_cb;
	struct list_head wheel_entry;
};

void timer_init_oneshot(struct timer * timer, timer_cb cb, unsigned long timeout);
void timer_init_cont(struct timer * timer, timer_cb cb, unsigned long period);
void timer_add(struct timer * timer);
void timer_del(struct timer * timer);
void timer_del_sync(struct timer * timer);
void timer_run_pending(void);
void timer_num_pending(struct timer * timer);

#endif
