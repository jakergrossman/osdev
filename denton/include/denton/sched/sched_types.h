#ifndef __DENTON_SCHED_TYPES_H
#define __DENTON_SCHED_TYPES_H

#include <stdint.h>

typedef int (*taskfn_t)(void* token);

typedef uint64_t pid_t;

#endif
