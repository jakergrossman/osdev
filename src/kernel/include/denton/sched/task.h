#ifndef __DENTON_SCHED_TASK_H
#define __DENTON_SCHED_TASK_H

#include "asm/sched/task.h"
#include "denton/list.h"
#include <denton/sched/sched_types.h>

#include <stdint.h>

enum {
	TASK_NAME_SIZE = 128,
};


enum task_state {
	TASK_ST_NONE,
	TASK_ST_NEW,
	TASK_ST_SLEEPING,
	TASK_ST_SLEEPING_INTR,
	TASK_ST_BLOCKED,
	TASK_ST_BLOCKED_INTR,
	TASK_ST_RUNNING,
	TASK_ST_STOPPED,
	TASK_ST_ZOMBIE,
	TASK_ST_DEAD,
};

struct task {
	struct arch_task_context arch_context;
	taskfn_t fn;
	void* privdata;

	pid_t pid;
	enum task_state state;
	void* stack;
	struct list_head tasklist;

	struct task* parent;
	char name[TASK_NAME_SIZE];

	uint64_t last_ran_ticks;
	uint64_t ran_ticks;
	uint64_t wake_tick;

	bool need_resched;
};

int           task_init(const char* name, taskfn_t fn, void* token, struct task* outp);
void          task_deinit(struct task* task);
int           task_init_user(const char* name, taskfn_t fn, struct task* outp);
void          task_set_state(struct task* task, enum task_state state);

#endif
