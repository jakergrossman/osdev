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
#include <denton/version.h>

#include <asm/irq.h>
#include <asm/cpu.h>
#include <asm/paging.h>

#include <limits.h>
#include <stdatomic.h>
#include <stdint.h>

static struct task kernel_init_task;

/* run early init functions in pre-scheduler environment */
static void* kernel_early_init(void)
{
	void (**init_fn)(void);
	for (init_fn = __initcalls; *init_fn; init_fn++) {
		if (*init_fn == __init_core) {
			break;
		}
		(*init_fn)();
	}
	return init_fn;
}

/* run init functions in post-scheduler environment */
static int kernel_init(void* start)
{
	void (**init_fn)(void);
	for (init_fn = start; *init_fn; init_fn++) {
		(*init_fn)();
	}
	return 0;
}

void kmain(void)
{
	// for now, update terminal base now that we are using the kernel pgdir
	terminal_update_base(INIT_VGA);

	sched_init();

	/* start of initcalls to run post-scheduler */
	void* after_sched_inits = kernel_early_init();

	task_init("kernel_init", kernel_init, after_sched_inits, &kernel_init_task);
	sched_add(&kernel_init_task);

	sched_start();
}
