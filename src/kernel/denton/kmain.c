#include "asm/cpu.h"
#include "asm/paging.h"
#include "asm/timer.h"
#include "denton/heap.h"
#include "denton/mm/mm_types.h"
#include "denton/sched.h"
#include "denton/sched/task.h"
#include "denton/sync/sem.h"
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
	return -1;
	
	for (size_t i; i < 500; i++) {
		// uintptr_t step = (uintptr_t)intervalp;
		// tsleep(step);
		atomic_fetch_add(&__cont, 1);
		// sem_up(&sem);
		klog_info("up %d\n", (uintptr_t)intervalp);
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

	struct task* producer = kmalloc(PAGE_SIZE, PGF_KERNEL);
	task_init("p1", producer_task, (void*)(200), producer);
	struct task* p2 = kmalloc(PAGE_SIZE, PGF_KERNEL);
	task_init("p2", producer_task, (void*)(4), p2);
	struct task* c1 = kmalloc(PAGE_SIZE, PGF_KERNEL);
	task_init("kernel_bar", consumer_task, "c1", c1);
	struct task* c2 = kmalloc(PAGE_SIZE, PGF_KERNEL);
	task_init("kernel_bar", consumer_task, "c2", c2);
	struct task* c3 = kmalloc(PAGE_SIZE, PGF_KERNEL);
	task_init("kernel_bar", consumer_task, "c3", c3);

	// sched_add(c1);
	// sched_add(c2);
	sched_add(producer);
	sched_add(p2);

	sched_start();
}
