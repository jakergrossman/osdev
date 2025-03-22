#ifndef __DENTON_SCHED_RQ_H
#define __DENTON_SCHED_RQ_H

#include <denton/sched/task.h>

struct sched_class;

struct sched_class {
	void (*enqueue)(struct sched_class * cls, struct task * task);
	void (*dequeue)(struct sched_class * cls, struct task * task);
	void (*yield_task)(struct sched_class * cls);
	void (*yield_to_task)(struct sched_class * cls, struct task * task);
	struct task * (*pick_task)(struct sched_class * cls);
};

static inline void sched_class_enqueue(struct sched_class * cls, struct task * task)
{
	cls->enqueue(cls, task);
}

static inline void sched_class_dequeue(struct sched_class * cls, struct task * task)
{
	cls->dequeue(cls, task);
}

static inline void sched_class_yield_task(struct sched_class * cls)
{
	cls->yield_task(cls);
}

static inline void sched_class_yield_to_task(struct sched_class * cls, struct task * task)
{
	cls->yield_to_task(cls, task);
}

static inline struct task * sched_class_pick_task(struct sched_class * cls)
{
	return cls->pick_task(cls);
}

#endif
