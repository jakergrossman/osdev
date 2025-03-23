#include "asm-generic/cpu.h"
#include "asm/cpu.h"
#include "asm/irq.h"
#include "asm/sched/task.h"
#include "asm/timer.h"
#include "denton/container_of.h"
#include "denton/heap.h"
#include "denton/klog.h"
#include "denton/mm/mm_types.h"
#include "denton/sched.h"
#include "denton/spinlock.h"
#include "denton/time/timer.h"
#include <denton/list.h>
#include <denton/sched/task.h>
#include <asm/sync/spinlock.h>

/** round robin scheduler */
struct rr_sched {
	spinlock_t lock;
	struct list_head ready_list;
	struct list_head notready_list;
	struct list_head sleep_list;
	struct task* __idle;
};

static void __rr_enqueue(struct rr_sched * sched, struct task * task)
{
	list_add_tail(&task->tasklist, &sched->ready_list);
}

static void __rr_dequeue(struct rr_sched * sched, struct task * task)
{
	list_del(&task->tasklist);
}

static void __rr_block(struct rr_sched * sched, struct task * task)
{
	list_add(list_del(&task->tasklist), &sched->notready_list);
}

static void __rr_unblock(struct rr_sched * sched, struct task * task)
{
	list_add_tail(list_del(&task->tasklist), &sched->ready_list);
}

static struct task * __rr_pick_task(struct rr_sched * sched)
{
	struct task * next = list_first_entry_or_null(&sched->ready_list, struct task, tasklist);
	list_rotate(&sched->ready_list);
	return next;
}

static struct rr_sched __sched = {
	.lock = SPINLOCK_INIT(0),
	.ready_list = LIST_HEAD_INIT(__sched.ready_list),
	.notready_list = LIST_HEAD_INIT(__sched.notready_list),
};

static int __sched_idle(void* token)
{
	while (1) {
		cpu_halt();
	}
}

int sched_init(void)
{
	int ret = 0;

	__sched.__idle = kmalloc(sizeof(struct task), PGF_KERNEL);
	ret = task_init("rr idle", __sched_idle, NULL, __sched.__idle);
	if (ret) {
		return ret;
	}

	return 0;
}

int sched_add(struct task * task)
{
	irq_flags_t flags;
	spin_lock_irqsave(&__sched.lock, &flags);
	__rr_enqueue(&__sched, task);
	spin_unlock_restore(&__sched.lock, flags);
	return 0;
}

static void __sched_switch(struct task * prev, struct task * next)
{
	if (prev) {
		arch_task_context_save(&prev->arch_context);
	}

	cpu_get_local()->current = next;

	arch_task_context_restore(&next->arch_context);
}

void sched_schedule(void)
{
	irq_flags_t flags;
	spin_lock_irqsave(&__sched.lock, &flags);

	struct task * prev = cpu_get_local()->current;
	struct task * next = __rr_pick_task(&__sched);
	if (!next) {
		next = __sched.__idle;
	}

	spin_unlock_restore(&__sched.lock, flags);

	__sched_switch(prev, next);
}

void sched_task_entry(void)
{
	struct task* entering = cpu_get_local()->current;
	taskfn_t fn = entering->fn;

	int code = fn(entering->privdata);
	klog_info("%s exit: %d\n", entering->name, code);
	__rr_dequeue(&__sched, entering);
	sched_schedule();
}

void sched_start(void)
{
	irq_enable();

	sched_schedule();

	while (1);
}

struct sched_timeout_timer {
	struct timer timer;
	struct task* task;
};

static struct sched_timeout_timer * from_timer(struct timer * timer)
{
	return container_of(timer, struct sched_timeout_timer, timer);
}

static void sched_process_timeout(struct timer * timer)
{
	struct sched_timeout_timer* tt = from_timer(timer);
	__rr_unblock(&__sched, tt->task);
}

long __sched_timeout(long timeout)
{
	struct sched_timeout_timer timer;
	unsigned long expire = timeout + timer_get_ticks();

	timer.task = cpu_get_local()->current;
	timer_init_oneshot(&timer.timer, sched_process_timeout, timeout);
	timer_add(&timer.timer);

	not_using_spin_lock(&__sched.lock) {
		sched_schedule();
	}

	timeout = expire - timer_get_ticks();

	return (timeout < 0) ? 0 : timeout;
}

long sched_timeout(long timeout)
{
	using_spin_lock(&__sched.lock) {
		__rr_block(&__sched, cpu_get_local()->current);
		return __sched_timeout(timeout);
	}
}

void sched_block(void)
{
	using_spin_lock(&__sched.lock) {
		__rr_block(&__sched, cpu_get_local()->current);
	}

	sched_schedule();
}

void sched_unblock(struct task * task)
{
	using_spin_lock(&__sched.lock) {
		__rr_unblock(&__sched, task);
	}
}
