#include "asm/timer.h"

#include <denton/sched.h>
#include <denton/sched/sleep.h>

void sleep_ms(long millis)
{
	// FIXME: overflow?
	long ticks = millis * TIMER_TICKS_PER_SECOND / 1000;
	while (ticks) {
		ticks = sched_timeout(ticks);
	}
}
