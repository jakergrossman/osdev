#ifndef __ARCH_I386_ASM_SCHED_TASK_H
#define __ARCH_I386_ASM_SCHED_TASK_H

#include <denton/sched/sched_types.h>
#include <stdint.h>

struct task;

struct arch_task_context {
	uint32_t esp;
	uint32_t ebi;
	uint32_t cr3;
} __packed;

void arch_task_setup_stack(struct task* t, taskfn_t task_code, void* token);

void arch_task_switch(struct arch_task_context* old, struct arch_task_context* new);

#endif
