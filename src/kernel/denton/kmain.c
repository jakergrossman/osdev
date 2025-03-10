#include "asm/cpu.h"
#include "asm/instr.h"
#include "asm/paging.h"
#include "asm/timer.h"
#include "denton/heap.h"
#include "denton/mm/mm_types.h"
#include "denton/sched.h"
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
	while (1) {
		irq_disable();
		klog_info("HI FROM ANOTHER STACK\n");
		irq_enable();
		for (volatile int i = 0; i < 0xFFFFF; i++);
		pause();
	}
	return 0;
}

static int bar(void* foo)
{
	while (1) {
		irq_disable();
		klog_info("HI FROM BAR STACK\n");
		irq_enable();
		for (volatile int i = 0; i < 0xFFFFF; i++);
	}
	return 0;
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
	
	sched_init();

	struct task* bartask = kmalloc(PAGE_SIZE, PGF_KERNEL);
	task_init("kernel_bar", bar, NULL, bartask);

	sched_add(task);
	sched_add(bartask);
	sched_start();

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
