#include "asm-generic/cpu.h"
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
#include <denton/math.h>

#include <asm/cpu.h>

#include <stdlib.h>

struct scheduler {
	spinlock_t lock;
	struct {
		struct list_head ready;
		struct list_head sleeping;
		struct list_head blocked;
	} tasks;
	struct task* running;
} __sched = {
	.lock = SPINLOCK_INIT(0),
	.tasks = {
		.ready = LIST_HEAD_INIT(__sched.tasks.ready),
		.blocked = LIST_HEAD_INIT(__sched.tasks.blocked),
	},
	.running = NULL,
};

enum sched_type {
	SCHED_IDLE = -1,
	SCHED_NONE,
	SCHED_PREEMPT,
	SCHED_BLOCK,
};

void __sched_set_state(enum task_state state)
{
	WRITE_ONCE(cpu_get_local()->current->state, state);
}

static void __sched_pick_next_task(void)
{
	struct task* iter = NULL;
	struct task* next = NULL;
	list_for_each_entry_safe(iter, &__sched.tasks.ready, next, tasklist) {
		list_del(&iter->tasklist);
		switch (iter->state) {
			case TASK_ST_RUNNING:
				break;
			case TASK_ST_NEW:
				__sched.running = iter;
				break;
			case TASK_ST_BLOCKED:
			default:
				list_add_tail(&iter->tasklist, &__sched.tasks.ready);
				break;
		}

		if (iter->state == TASK_ST_RUNNING) {
			__sched.running = iter;
			__sched.running->last_ran_ticks = timer_get_ticks();
			cpu_get_local()->current = iter;
			break;
		}
	}
}

static void __sched_switch(struct task * prev, struct task * next)
{
	if (prev) {
		arch_task_context_save(&prev->arch_context);
	}

	cpu_get_local()->current = next;
	cpu_preempt_enable();

	arch_task_context_restore(&next->arch_context);
}

/**
 * preconditions:
 * - must be called with preemption disabled
 * - current task state is set to incoming state
 */
static void __schedule(void)
{
	struct task* prev = cpu_get_local()->current;

	struct task* iter = NULL;
	struct task* next = NULL;
	list_for_each_entry_safe(iter, &__sched.tasks.ready, next, tasklist) {
		list_rotate(&__sched.tasks.ready);
		switch (iter->state) {
			case TASK_ST_RUNNING:
				break;
			case TASK_ST_NEW:
				__sched.running = iter;
				cpu_get_local()->current = iter;
				__sched_set_state(TASK_ST_RUNNING);
				cpu_preempt_enable();
				arch_task_entry(iter);
				break;
			default:
				klog_error("no task?\n");
				break;
		}

		__sched.running = iter;
		__sched.running->last_ran_ticks = timer_get_ticks();
		break;
	}

	__sched_switch(prev, __sched.running);
}

static void __sched_yield(struct list_head * to_list);
void __sched_move_task(struct scheduler * sched, struct task * task, struct list_head * list);

void __sched_add(struct scheduler* sched, struct task* ktask)
{
	spin_lock(&sched->lock);

	list_add_tail(&ktask->tasklist, &sched->tasks.ready);

	spin_unlock(&sched->lock);
}

/** select a new "running" task */
int __sched_reschedule(struct scheduler* sched, struct list_head * to_list)
{
	if (sched->running) {
		uint64_t now = timer_get_ticks();
		uint64_t delta = now - __sched.running->last_ran_ticks;
		__sched.running->ran_ticks += kmax(delta, 1);
		sched->running->last_ran_ticks = now;

		if (sched->running != &__sched_idle) {
			list_add_tail(&sched->running->tasklist, to_list);
		}
		sched->running = NULL;
	}

	// list_for_each_entry_safe(iter, &sched->tasks.sleeping, next, tasklist) {
	// 	if (timer_get_ticks() > iter->wake_tick) {
	// 		__sched_move_task(sched, iter, &sched->tasks.ready);
	// 	}
	// 	break;
	// }

	return 0;
}

void __sched_move_task(struct scheduler * sched, struct task * task, struct list_head * list)
{
	if (list_placed_in_list(&task->tasklist)) {
		list_del(&task->tasklist);
	}

	list_add_tail(&task->tasklist, list);
}

static inline bool sched_need_resched(void)
{
	struct task* current = cpu_get_local()->current;
	return current->need_resched;
}

void sched_schedule(void)
{
	do {
		cpu_preempt_disable();
		__schedule();
		cpu_preempt_enable();
	} while (sched_need_resched());
}

/** move the current task into the blocked state */
void sched_block(void)
{
	if (__sched.running) {
		__sched.running->state = TASK_ST_SLEEPING_INTR;
	}
	__sched_yield(&__sched.tasks.blocked);
}

static void __sched_unblock(struct task * task)
{
	task->state = TASK_ST_RUNNING;
	__sched_move_task(&__sched, task, &__sched.tasks.ready);
}

void sched_unblock(struct task * task)
{
	irq_flags_t flags;
	spin_lock_irqsave(&__sched.lock, &flags);
	__sched_unblock(task);
	spin_unlock_restore(&__sched.lock, flags);
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
	int ret;
	ret = task_init("scheduler idle", sched_idle, NULL, &__sched_idle);
	if (ret) {
		return ret;
	}
	sched_add(&__sched_idle);
	cpu_get_local()->scheduler.cr3 = cpu_read_cr3();
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

static void __sched_yield(struct list_head * to_list)
{
	cli();
	if (__sched.running) {
		arch_task_context_save(&__sched.running->arch_context);
	}

	__sched_reschedule(&__sched, to_list);

	arch_task_context_restore(&__sched.running->arch_context);
	sti();
}

void sched_yield(void)
{
	__sched_yield(&__sched.tasks.ready);
}

void sched_reschedule(void);
void sched_exit(int status);

void sched_task_entry(void)
{
	struct task* entering = cpu_get_local()->current;
	taskfn_t fn = entering->fn;

	sched_exit(fn(entering->privdata));
}

