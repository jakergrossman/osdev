#include "asm-generic/cpu.h"
#include "asm/cpu.h"
#include "asm/irq.h"
#include "asm/sched/task.h"
#include "asm/timer.h"
#include "denton/heap.h"
#include "denton/klog.h"
#include "denton/mm/mm_types.h"
#include "denton/spinlock.h"
#include <denton/list.h>
#include <denton/sched/task.h>
#include <asm/sync/spinlock.h>

/** round robin scheduler */
struct rr_sched {
	spinlock_t lock;
	struct list_head ready_list;
	struct list_head notready_list;
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
	const unsigned int dur = 1000;
	static unsigned int uptime = 0;
	while (1) {
		unsigned long up_ms = timer_get_ms();
		if (up_ms >= uptime+dur) {
			klog_info("uptime: %ld.%03ld\n", up_ms / 1000, up_ms % 1000);
			uptime = up_ms;
		}
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

	while (1) {
		cpu_halt();
	}
}
