#include "asm/cpu.h"
#include "asm/gdt.h"
#include "asm/paging.h"
#include <denton/sched.h>
#include <denton/compiler.h>
#include <asm/sched/task.h>
#include <string.h>

/** set up the stack in a way it can be 'arch_task_context_restore'd */
static void arch_task_setup_stack(struct task * task)
{
	uint32_t* ptr = (uint32_t*)task->arch_context.esp;

	ptr--;
	*ptr = 0xDEADBEEF;
	ptr--;
	*ptr = task->arch_context.eip;
	ptr--;
	*ptr = task->arch_context.ebp;
	task->arch_context.esp = (uint32_t)ptr;
	task->arch_context.ebp = (uint32_t)ptr;
}

/**
 * arch_task_init() - initialize a kernel tasks stack to be ready for scheduling
 * @t: task to initialize
 * @task_code: code for task
 * @token: argument for @task_code
 *
 */
void arch_task_init(struct task* t)
{
	uintptr_t sp = (uintptr_t)t->stack;
	memset((void*)sp, 0, PAGE_SIZE); // FIXME: task stack size always 4096?

	/* stack grows down */
	t->arch_context.ebp = sp + PAGE_SIZE - 1;
	t->arch_context.stack_frame_top = t->arch_context.ebp;

	t->arch_context.esp = sp + PAGE_SIZE - 1;
	t->arch_context.stack_top = t->arch_context.esp;

	t->arch_context.eip = (uintptr_t)sched_task_entry;
	t->arch_context.cs = GDT_KERNEL_CODE << 3;
	t->arch_context.ds = GDT_KERNEL_DATA << 3;
	t->arch_context.eflags = (0 << 12) | (1 << 9);

	arch_task_setup_stack(t);
}
