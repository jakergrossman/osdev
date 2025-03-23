#include "asm-generic/cpu.h"
#include "asm/sync/spinlock.h"
#include "asm/timer.h"
#include "denton/spinlock.h"
#include <denton/list.h>

#include <denton/time/timer.h>

struct timer_wheel {
	spinlock_t lock;
	struct list_head timers;
	struct timer* running;
};

#define for_each_timer_safe(wheel, iter, next) \
	list_for_each_entry_safe(iter, &(wheel)->timers, next, wheel_entry)

static struct timer_wheel __timer_wheel = {
	.lock = SPINLOCK_INIT(0),
	.timers = LIST_HEAD_INIT(__timer_wheel.timers),
	.running = NULL,
};

void timer_init_cont(struct timer * timer, timer_cb cb, unsigned long period)
{
	unsigned long now = timer_get_ticks();
	timer->mode = TIMER_CONTINUE;
	timer->expires = now + period;
	timer->period = period;
	timer->expire_cb = cb;
}

void timer_init_oneshot(struct timer * timer, timer_cb cb, unsigned long timeout)
{
	unsigned long now = timer_get_ticks();
	timer->mode = TIMER_ONESHOT;
	timer->expires = now + timeout;
	timer->expire_cb = cb;
}

void __timer_add(struct timer * timer)
{
	struct list_head* entry = &__timer_wheel.timers;
	if (!list_empty(&__timer_wheel.timers)) {
		struct timer* iter;
		struct timer* next;
		
		/** find next place to put timer */
		list_for_each_entry_safe(iter, &__timer_wheel.timers, next, wheel_entry) {
			if (timer->expires <= iter->expires) {
				entry = &iter->wheel_entry;
				break;
			}
		}
	}

	list_add_tail(&timer->wheel_entry, entry);
}

void timer_add(struct timer * timer)
{
	using_spin_lock(&__timer_wheel.lock) {
		__timer_add(timer);
	}
}

void timer_del(struct timer * timer)
{
	using_spin_lock(&__timer_wheel.lock) {
		list_del(&timer->wheel_entry);
	}
}

void timer_del_sync(struct timer * timer)
{
	using_spin_lock(&__timer_wheel.lock) {
		if (__timer_wheel.running != timer) {
			list_del(&timer->wheel_entry);
		}

		not_using_spin_lock(&__timer_wheel.lock) {
			cpu_relax();
		}
	}
}

void timer_run_pending(void)
{
	struct timer* iter;
	struct timer* next;
	unsigned long tick = timer_get_ticks();
	using_spin_lock(&__timer_wheel.lock) {
		for_each_timer_safe(&__timer_wheel, iter, next) {
			if (iter->expires > tick) {
				break;
			}

			__timer_wheel.running = iter;
			iter->expire_cb(iter);
			__timer_wheel.running = NULL;

			list_del(&iter->wheel_entry);
			if (iter->mode == TIMER_CONTINUE) {
				/* queue up another period */
				iter->expires += iter->period;
				__timer_add(iter);
			}
		}
	}
}
