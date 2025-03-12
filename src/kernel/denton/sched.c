#include "asm/instr.h"
#include "asm/irq.h"
#include "asm/sched/task.h"
#include "asm/timer.h"
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
	if (sched->running) {
		uint64_t now = timer_get_ticks();
		__sched.running->ran_ticks += now - __sched.running->last_ran_ticks + 1;
		sched->running->last_ran_ticks = now;
		list_add_tail(&sched->running->tasklist, &sched->ready_list);
		sched->running = NULL;
	}

	if (list_empty(&sched->ready_list)) {
		return -ENOENT;
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
				if (timer_get_ticks() > iter->wake_tick) {
					iter->state = TASK_ST_RUNNING;
				} else {
					list_add_tail(&iter->tasklist, &sched->ready_list);
				}
				break;
			default:
				list_add_tail(&iter->tasklist, &sched->ready_list);
				break;
		}

		if (iter->state == TASK_ST_RUNNING) {
			sched->running = iter;
			sched->running->last_ran_ticks = timer_get_ticks();
			cpu_get_local()->current = iter;
			break;
		}
	}

	return 0;
}


void sched_exit(int status)
{
	struct task* task = cpu_get_local()->current;
	klog(status ? KLOG_TRACE : KLOG_WARN,
	     "task \"%s\" exited with code: %d\n",
	     task->name, status);
}

static int sched_idle(void* token)
{
	static uint64_t thresh = TIMER_TICKS_PER_SECOND;
	while (1) {
		if (cpu_get_local()->current->ran_ticks  >= thresh) {
			thresh += TIMER_TICKS_PER_SECOND;
			klog_info("Run for %llu ticks\n", cpu_get_local()->current->ran_ticks);
		}
		hlt();
	}
}

struct task __sched_idle;

int sched_init(void)
{
	int ret;
	ret = task_init("scheduler idle", sched_idle, NULL, &__sched_idle);
	if (ret) {
		return ret;
	}
	cpu_get_local()->scheduler.cr3 = cpu_read_cr3();
	sched_add(&__sched_idle);
	return 0;
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
	cli();
	if (__sched.running) {
		arch_task_context_save(&__sched.running->arch_context);
	}

	__sched_reschedule(&__sched);

	arch_task_context_restore(&__sched.running->arch_context);
	sti();
}
void sched_reschedule(void);
void sched_exit(int status);

void sched_task_entry(void)
{
	struct task* entering = cpu_get_local()->current;
	taskfn_t fn = entering->fn;

	sched_exit(fn(entering->privdata));
}

void sched_sleep_ms(uint32_t ms)
{
	unsigned long now = timer_get_ticks();
	unsigned long then = now + (ms * TIMER_TICKS_PER_SECOND / 1000);

	struct task* current = cpu_get_local()->current;
	current->wake_tick = then;
	current->state = TASK_ST_SLEEPING;

	sched_yield();
}
