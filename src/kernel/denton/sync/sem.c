#include "asm/cpu.h"
#include "denton/sched.h"
#include "denton/sched/task.h"
#include <denton/sync/sem.h>
#include <denton/spinlock.h>
#include <denton/list.h>
#include <denton/errno.h>
#include <denton/compiler.h>

#include <asm/irq.h>


struct sem_waiter {
	struct list_head wait_entry;
	struct task* waiting_task;
	bool up;
};

void sem_init(sem_t* sem, long count)
{
	*sem = (sem_t) {
		.lock = SPINLOCK_INIT(0),
		.count = count,
		.waitlist = LIST_HEAD_INIT(sem->waitlist),
	};
}

/** wake up the next waiting task, which is assumed to exist */
static inline void __sem_up(sem_t* sem)
{
	/* get and remove next item */
	struct sem_waiter* next = list_first_entry(&sem->waitlist, struct sem_waiter, wait_entry);
	list_del(&next->wait_entry);

	next->up = true;
	sched_unblock(next->waiting_task);
}

void sem_up(sem_t* sem)
{
	irq_flags_t flags;
	spin_lock_irqsave(&sem->lock, &flags);

	if (likely(list_empty(&sem->waitlist))) {
		sem->count += 1;
	} else {
		__sem_up(sem);
	}

	spin_unlock_restore(&sem->lock, flags);
}

static inline int __sem_down(sem_t* sem)
{
	struct sem_waiter waiter;
	list_add_tail(&waiter.wait_entry, &sem->waitlist);
	waiter.waiting_task = cpu_get_local()->current;

	for (;;) {
		// TODO: signalling
		spin_unlock(&sem->lock);
		__sched_set_state(TASK_ST_BLOCKED_INTR);
		sched_schedule();
		spin_lock(&sem->lock);
		if (waiter.up) {
			return 0;
		}
	}

	return 1;
}

int sem_down(sem_t* sem)
{
	irq_flags_t flags;
	spin_lock_irqsave(&sem->lock, &flags);

	int status = 0;
	if (likely(sem->count > 0)) {
		sem->count -= 1;
	} else {
		status = __sem_down(sem);
	}

	spin_unlock_restore(&sem->lock, flags);

	return status;
}

bool sem_trydown(sem_t* sem)
{
	irq_flags_t flags;

	spin_lock_irqsave(&sem->lock, &flags);
	long new_count = sem->count - 1;
	bool can_take = (new_count >= 0);

	if (likely(can_take)) {
		sem->count = new_count;
	}

	spin_unlock_restore(&sem->lock, flags);

	return can_take;
}
