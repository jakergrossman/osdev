#include "asm/cpu.h"
#include "asm/gdt.h"
#include "asm/paging.h"
#include <denton/sched.h>
#include <denton/compiler.h>
#include <asm/sched/task.h>
#include <string.h>

// /** spawn point for all tasks, ever */
// int __naked arch_task_entry(taskfn_t task_code)
// {
// 	asm volatile (
// 		"	pop %ebx\n"        /* pull task_code off the stack */
// 		"	sti\n"
// 		"	call *%ebx\n"      /* call the kernel thread function. */
// 		"	cli\n"             /* disable interrupts to prep for killing task */
// 		"	push %eax\n"       /* %eax is returned from the thread, */
// 		"	call sched_exit\n" /* forward it to thread exit code */
// 		""
// 		"   cli\n"             /* should never get here, spin forever */
// 		"1: loop 1b\n"
// 	);
// }

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
}
