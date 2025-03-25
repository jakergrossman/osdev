#include <denton/heap.h>
#include <denton/mm/mm_types.h>
#include <denton/sched.h>
#include <denton/sched/task.h>
#include <denton/time/timer.h>
#include <denton/bits.h>
#include <denton/tty.h>
#include <denton/klog.h>
#include <denton/list.h>
#include <denton/compiler.h>
#include <denton/ring.h>
#include <denton/initcall.h>

#include <asm/irq.h>
#include <asm/cpu.h>
#include <asm/paging.h>

#include <limits.h>
#include <stdatomic.h>
#include <stdint.h>

static struct task kernel_init_task;

static int kernel_init(void* unused)
{
	void (**init_fn)(void);
	for (init_fn = __initcalls; *init_fn; init_fn++) {
		(*init_fn)();
	}
}

void kmain(void)
{
	// for now, update terminal base now that we are using the kernel pgdir
	terminal_update_base(INIT_VGA);

	klog_info("OS is running...\n");

	sched_init();

	task_init("kernel_init", kernel_init, NULL, &kernel_init_task);
	sched_add(&kernel_init_task);

	sched_start();
}
