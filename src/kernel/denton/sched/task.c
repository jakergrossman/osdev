#include "asm-generic/rwonce.h"
#include "denton/heap.h"
#include "denton/sched.h"
#include <denton/mm/mm_types.h>
#include <denton/sched/task.h>
#include <denton/errno.h>

#include <asm/sched/task.h>
#include <asm/paging.h>

#include <stdatomic.h>
#include <string.h>

static _Atomic(unsigned long) __next_task_pid;

static uint32_t task_next_pid(void)
{
	return atomic_fetch_add(&__next_task_pid, 1);
}

/**
 * task_init() - make a task ready to run
 * @name: friendly name of the task
 * @fn: task code
 * @outp: the task structure to initialize
 *
 * Returns 0 on success or a negative error code on failure
 */
int task_init(const char* name, taskfn_t fn, void* token, struct task* outp)
{
	memset(outp, 0, sizeof(*outp));

	void* stack = kmalloc(PAGE_SIZE, PGF_KERNEL);
	if (!stack) {
		return -ENOMEM;
	}

	outp->stack = stack;
	outp->privdata = token;
	arch_task_init(outp);

	strncpy(&outp->name[0], name, sizeof(outp->name));

	outp->pid = task_next_pid();
	outp->state = TASK_ST_NEW;
	outp->fn = fn;

	return 0;
}

void task_set_state(struct task* task, enum task_state state)
{
	WRITE_ONCE(task->state, state);
}

/**
 * task_deinit() - deallocate the inner resources of a task
 * @task: task to deallocate
 */
void task_deinit(struct task* task);

/**
 * task_init() - make a userspace task ready to run
 * Returns 0 on success or a negative error code on failure
 */
int task_init_user(const char* name, taskfn_t fn, struct task* outp)
{
	return -ENOSYS;
}

