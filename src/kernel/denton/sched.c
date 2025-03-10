#include "asm/irq.h"
#include "asm/sched/task.h"
#include "denton/klog.h"
#include "denton/list.h"
#include <denton/sched.h>
#include <denton/types.h>
#include <denton/spinlock.h>

#include <asm/cpu.h>

static LIST_HEAD(__sched_tasklist);
static SPIN_LOCK(__sched_lock, 0);

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

	arch_task_switch(&cpu_get_local()->scheduler, &next->arch_context);

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
	using_spin_lock(&__sched_lock) {
		list_add_tail(&task->tasklist, &__sched_tasklist);
	}
	return 0;
}

void sched_yield(void)
{
	struct task* t = cpu_get_local()->current;

	using_spin_lock(&__sched_lock) {
		arch_task_switch(&t->arch_context, &cpu_get_local()->scheduler);
	}
}
void sched_reschedule(void);
void sched_exit(int status);

void sched_task_entry(void)
{
	spin_unlock(&__sched_lock);
}
