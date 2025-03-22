#ifndef __DENTON_SCHED_H
#define __DENTON_SCHED_H

#include "asm/sync/spinlock.h"
#include "sys/compiler.h"
#include <denton/sched/task.h>
#include <denton/compiler.h>

int  sched_init(void);              /* initiialize internal schedular data structures */
void sched_start(void) __noreturn;  /* start the scheduler */
int  sched_add(struct task* task);  /* add a task to the list of schedulable tasks */
void sched_yield(void);             /* yield the current task, and pick a task to run */
void sched_reschedule(void);        /* pick a task to run */
void sched_exit(int status);        /* called by task exit procedure */
void sched_task_entry(void);
void sched_block(void);
void sched_unblock(struct task * task);
void sched_timer_reschedule(void);

/** initiialize the scheduler */
int  sched_init(void);

/** add an already initialized task to the scheduler */
int  sched_add(struct task * task);

/**
 * sched_create() - initialize and add a task to the scheduler
 * @fn: task code
 * @token: task-private data argument for @fn
 * @namefmt: printf format for human-friendly 
 * @...: arguments for @namefmt
 */
struct task * __format(3, 4)
sched_create(taskfn_t fn, void * token, const char * namefmt, ...);

void sched_schedule(void);

/**
 * sched_tick() - called by the timer code with SCHED_HZ frequency
 *
 * called with interrupts disabled
 */
void sched_tick(void);

/** sleep for @ticks */
int  sched_sleep_ticks(int ticks);
int  sched_sleep_ticks_interruptible(int ticks);
int  sched_sleep_ms(int ticks);
int  sched_sleep_ms_interruptible(int ticks);

void __sched_entry_point(void);
void __sched_set_state(enum task_state state);

#endif
