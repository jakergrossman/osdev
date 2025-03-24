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

#include <asm/irq.h>
#include <asm/cpu.h>
#include <asm/paging.h>

#include <limits.h>
#include <stdatomic.h>
#include <stdint.h>

void kmain(void)
{
	// for now, update terminal base now that we are using the kernel pgdir
	terminal_update_base(INIT_VGA);

	klog_info("OS is running...\n");

	struct task* task = kmalloc(PAGE_SIZE, PGF_KERNEL);
	if (!task) {
		cpu_halt();
		//panic();
	}

	sched_init();

	sched_start();
}
