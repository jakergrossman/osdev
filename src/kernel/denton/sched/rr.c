
#include "asm/sync/spinlock.h"
#include <denton/sched/rr.h>

const struct sched_class rr_sched_class = {
};

/** round robin scheduler */
struct rr_sched {
	spinlock_t lock;
	struct list_head ready_list;
	struct list_head notready_list;
};

void rr_enqueue(struct rr_sched * sched, struct task * task)
{
}

void rr_dequeue(struct rr_sched * sched, struct task * task)
{
}
