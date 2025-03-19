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
	static uint64_t thresh = TIMER_TICKS_PER_SECOND;
	while (1) {
		if (cpu_get_local()->current->ran_ticks  >= thresh) {
			thresh += TIMER_TICKS_PER_SECOND;
			klog_info("Run for %llu ms\n", 1000 * cpu_get_local()->current->ran_ticks / TIMER_TICKS_PER_SECOND);
		}
		sched_sleep_ms(20);
	}
	return 0;
}

static int bar(void* foo)
{
	static uint64_t thresh = TIMER_TICKS_PER_SECOND;
	while (1) {
		if (cpu_get_local()->current->ran_ticks  >= thresh) {
			thresh += TIMER_TICKS_PER_SECOND;
			klog_info("Run for %llu ms\n", 1000 * cpu_get_local()->current->ran_ticks / TIMER_TICKS_PER_SECOND);
		}
		sched_sleep_ms(10);
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
}
