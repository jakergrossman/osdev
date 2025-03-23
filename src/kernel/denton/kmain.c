#include "asm/cpu.h"
#include "asm/paging.h"
#include "asm/timer.h"
#include "denton/heap.h"
#include "denton/mm/mm_types.h"
#include "denton/sched.h"
#include "denton/sched/sleep.h"
#include "denton/sched/task.h"
#include "denton/sync/sem.h"
#include "denton/time/timer.h"
#include <denton/bits.h>
#include <denton/tty.h>
#include <denton/klog.h>
#include <denton/list.h>
#include <denton/compiler.h>
#include <denton/ring.h>

#include <asm/irq.h>

#include <limits.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdlib.h>

static SEM_DECL(sem, 0);

extern void msleep(unsigned long millis);
extern void tsleep(unsigned long ticks)
{;
	unsigned long now = timer_get_ticks();
	unsigned long then = now + ticks;

	struct task* current = cpu_get_local()->current;
	current->wake_tick = then;
	current->state = TASK_ST_SLEEPING;

	// sched_yield();
}

_Atomic(unsigned long) __cont = 0;

static int producer_task(void * intervalp)
{
	klog_info("starting\n");
	
	for (;;) {
		sleep_ms((long)intervalp);
		klog_info("%ld\n", (long)intervalp);
	}

	return (int)intervalp;
}

static int consumer_task(void * descp)
{
	klog_info("starting %s\n", (char*)descp);

	for (;;) {
		// sem_down(&sem);
		unsigned long val = atomic_load(&__cont);
		klog_info("%s: %ld\n", (char*)descp, val);
	}
}

void tcb(struct timer * timer)
{
	klog_info("tcb\n");
}

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

	struct task* p1 = kmalloc(PAGE_SIZE, PGF_KERNEL);
	task_init("p1", producer_task, (void*)(200), p1);
	struct task* p2 = kmalloc(PAGE_SIZE, PGF_KERNEL);
	task_init("p2", producer_task, (void*)(500), p2);

	sched_add(p1);
	sched_add(p2);

	sched_start();
}
