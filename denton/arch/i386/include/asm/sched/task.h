#ifndef __ARCH_I386_ASM_SCHED_TASK_H
#define __ARCH_I386_ASM_SCHED_TASK_H

#include <denton/sched/sched_types.h>
#include <denton/compiler.h>
#include <stdint.h>

struct task;

struct arch_task_regs {
};

struct arch_task_context {
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint32_t eip;
	uint32_t eflags;
	uint32_t cs;
	uint32_t ds;
	uint32_t cr3;
	uint32_t stack_top;
	uint32_t stack_frame_top;
	uint8_t  fpu_state[100];
} __packed;

void arch_task_init(struct task* t);

extern void arch_task_switch(struct arch_task_context* old, struct arch_task_context* new) __noreturn;
extern void arch_task_context_save(struct arch_task_context* ctx);
extern void arch_task_context_restore(struct arch_task_context* ctx);
extern void arch_task_entry(struct task* task);

#endif
