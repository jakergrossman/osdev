#include "asm/cpu.h"
#include <denton/sched.h>
#include <denton/compiler.h>
#include <asm/sched/task.h>

/** spawn point for all tasks, ever */
int __naked arch_task_entry(taskfn_t task_code)
{
	asm volatile (
		"	pop %ebx\n"        /* pull task_code off the stack */
		"	sti\n"
		"	call *%ebx\n"      /* call the kernel thread function. */
		"	cli\n"             /* disable interrupts to prep for killing task */
		"	push %eax\n"       /* %eax is returned from the thread, */
		"	call sched_exit\n" /* forward it to thread exit code */
		""
		"   cli\n"             /* should never get here, spin forever */
		"1: loop 1b\n"
	);
}

/**
 * arch_task_setup_stack() - initialize a kernel tasks stack to be ready for scheduling
 * @t: task to initialize
 * @task_code: code for task
 * @token: argument for @task_code
 */
void arch_task_setup_stack(struct task* t, taskfn_t task_code, void* token)
{
	void* sp = t->stack;

	sp -= sizeof(token);
	*(void**)sp = token;

	/* task_code(token) */
	sp -= sizeof(task_code);
	*(void**)sp = task_code;

	/* arch_task_entry(task_code); */
	sp -= sizeof(&arch_task_entry);
	*(void**)sp = arch_task_entry;

	/* 0 for ebp, edi, esi, ebx */
	for (size_t i = 0; i < 4; i++) {
		sp -= sizeof(uint32_t);
		*(uint32_t*)sp = 0;
	}

	t->stack = sp;
	t->arch_context.esp = (uint32_t)sp;
	t->arch_context.cr3 = cpu_read_cr3();
}
