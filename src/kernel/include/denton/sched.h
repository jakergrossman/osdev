#ifndef __DENTON_SCHED_H
#define __DENTON_SCHED_H

#include <denton/sched/task.h>

int  sched_init(void);              /* initiialize internal schedular data structures */
int  sched_start(void);             /* start the scheduler */
int  sched_add(struct task* task);  /* add a task to the list of schedulable tasks */
int  sched_yield(void);             /* yield the current task, and pick a task to run */
void sched_reschedule(void);        /* pick a task to run */
void sched_exit(int status);        /* called by task exit procedure */

#endif
