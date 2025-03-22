#ifndef __DENTON_SCHED_RR_H
#define __DENTON_SCHED_RR_H

#include "denton/container_of.h"
#include <denton/sched/rq.h>

struct rr_sched;

extern const struct sched_class rr_sched_class;

void rr_init(struct rr_sched* sched);

#endif
