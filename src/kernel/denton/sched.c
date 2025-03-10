#include "denton/klog.h"
#include <denton/sched.h>
#include <denton/types.h>

#include <asm/cpu.h>

struct list_head __sched_tasklist;

void sched_reschedule(void)
{
	struct task* current = cpu_get_local()->current;
	(void)current;
}

void sched_exit(int status)
{
	struct task* task = cpu_get_local()->current;
	klog_trace("task \"%s\" exited with code: %d\n",
	           task->name, status);
}
