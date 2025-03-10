#include "asm/cpu.h"
#include "asm/instr.h"
#include "asm/paging.h"
#include "asm/timer.h"
#include "denton/heap.h"
#include "denton/mm/mm_types.h"
#include "denton/sched/task.h"
#include <denton/bits.h>
#include <denton/tty.h>
#include <denton/klog.h>
#include <denton/list.h>
#include <denton/compiler.h>
#include <denton/ring.h>

#include <asm/irq.h>

#include <limits.h>
#include <stdlib.h>

static int foo(void* foo)
{
	klog_info("HI FROM ANOTHER STACK\n");
	return -1;
}

void kmain(void)
{
	// for now, update terminal base now that we are using the kernel pgdir
	terminal_update_base(INIT_VGA);

	klog_info("OS is running...\n");

	struct task* task = kmalloc(PAGE_SIZE, PGF_KERNEL);
	if (!task) {
		arch_cpu_halt();
		//panic();
	}

	if (task_init("kernel_init", foo, NULL, task)) {
		arch_cpu_halt();
	}

	struct task dummy;
#include "asm/sched/task.h"
	cpu_get_local()->current = task;
	arch_task_switch(&dummy.arch_context, &task->arch_context);


	sti();

	uint32_t next = 0;
	while (1) {
		uint32_t ms = timer_get_ms();
		if (ms >= next)
		{
			klog_error("%ld\n", (long)ms);
			next = ms + 250;
		}
	}

	klog_error("aaaand its dead\n");
}
