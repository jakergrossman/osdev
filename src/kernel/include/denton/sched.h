#ifndef __DENTON_SCHED_H
#define __DENTON_SCHED_H

#include <denton/sched/task.h>
#include <denton/compiler.h>

int  sched_init(void);              /* initiialize internal schedular data structures */
void sched_start(void) __noreturn;  /* start the scheduler */
int  sched_add(struct task* task);  /* add a task to the list of schedulable tasks */
void sched_yield(void);             /* yield the current task, and pick a task to run */
void sched_reschedule(void);        /* pick a task to run */
void sched_exit(int status);        /* called by task exit procedure */
void sched_task_entry(void);
void sched_sleep_ms(uint32_t ms);

#endif
