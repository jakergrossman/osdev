#include "asm/irq.h"
#include "asm/sched/task.h"
#include "denton/klog.h"
#include "denton/list.h"
#include "denton/sched/sched_types.h"
#include "denton/sched/task.h"
#include <denton/sched.h>
#include <denton/types.h>
#include <denton/spinlock.h>
#include <denton/errno.h>

#include <asm/cpu.h>

#include <stdlib.h>

static LIST_HEAD(__sched_tasklist);
static SPIN_LOCK(__sched_lock, 0);

struct scheduler {
	spinlock_t lock;
	struct list_head ready_list;
	struct list_head blocked_list;
	struct task* running;
} __sched = {
	.lock = SPINLOCK_INIT(0),
	.ready_list = LIST_HEAD_INIT(__sched.ready_list),
	.blocked_list = LIST_HEAD_INIT(__sched.blocked_list),
	.running = NULL,
};

void __sched_add(struct scheduler* sched, struct task* ktask)
{
	spin_lock(&sched->lock);

	list_add_tail(&ktask->tasklist, &sched->ready_list);

	spin_unlock(&sched->lock);
}

/** select a new "running" task */
int __sched_reschedule(struct scheduler* sched)
{
	if (list_empty(&sched->ready_list)) {
		return -ENOENT;
	}

	if (sched->running) {
		list_add_tail(&sched->running->tasklist, &sched->ready_list);
		sched->running = NULL;
	}

	struct task* iter = NULL;
	struct task* next = NULL;
	list_for_each_entry_safe(iter, &sched->ready_list, next, tasklist) {
		list_del(&iter->tasklist);
		switch (iter->state) {
			case TASK_ST_RUNNING:
				break;
			case TASK_ST_NEW:
				sched->running = iter;
				cpu_get_local()->current = iter;
				iter->state = TASK_ST_RUNNING;
				arch_task_entry(iter);
				break;
			case TASK_ST_SLEEPING:
			default:
				list_add_tail(&iter->tasklist, &sched->ready_list);
				break;
		}

		if (iter->state == TASK_ST_RUNNING) {
			sched->running = iter;
			cpu_get_local()->current = iter;
			break;
		}
	}

	return 0;
}

void sched_reschedule(void)
{
	// irq_flags_t flags = irq_save();
	irq_disable();

	/* simple round robin for now */
	struct task* old = cpu_get_local()->current;
	if (old) {
		list_add_tail(&old->tasklist, &__sched_tasklist);
	}

	struct task* next = list_first_entry(&__sched_tasklist, struct task, tasklist);
	list_del(&next->tasklist);

	cpu_get_local()->current = next;

	irq_flags_t flgs = irq_save();
	irq_disable();

	arch_task_switch(&cpu_get_local()->scheduler, &next->arch_context);

	irq_restore(flgs);
	irq_enable();

	// irq_restore(flags);
	irq_enable();
}

void sched_exit(int status)
{
	struct task* task = cpu_get_local()->current;
	klog(status ? KLOG_TRACE : KLOG_WARN,
	     "task \"%s\" exited with code: %d\n",
	     task->name, status);
}

int sched_init(void)
{
	cpu_get_local()->scheduler.cr3 = cpu_read_cr3();
	return 0;
}

int sched_start(void)
{
	spin_lock(&__sched_lock);

	irq_enable();
	while (1) {
		sched_reschedule();
	}
}

int sched_add(struct task* task)
{
	__sched_add(&__sched, task);
	// spin_lock(&__sched_lock);
	// list_add_tail(&task->tasklist, &__sched_tasklist);
	// spin_unlock(&__sched_lock);
	return 0;
}

void sched_yield(void)
{
	if (__sched.running) {
		arch_task_context_save(&__sched.running->arch_context);
	}

	__sched_reschedule(&__sched);

	arch_task_context_restore(&__sched.running->arch_context);
}
void sched_reschedule(void);
void sched_exit(int status);

void sched_task_entry(void)
{
	struct task* entering = cpu_get_local()->current;
	taskfn_t fn = entering->fn;

	sched_exit(fn(entering->privdata));
}
