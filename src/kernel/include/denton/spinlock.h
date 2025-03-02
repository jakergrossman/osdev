
#include "asm/instr.h"
#include "denton/atomic.h"
#include "denton/compiler.h"
#include <stdint.h>

typedef struct spinlock {
	uint32_t locked;
} spinlock_t;

#define SPINLOCK_INIT() { .locked = 0 }

static __always_inline void
spin_lock(spinlock_t* lock)
{
	while (__sync_val_compare_and_swap(&lock->locked, 0, 1) != 0)
		;
}

static __always_inline void
spin_unlock(spinlock_t* lock)
{
	lock->locked = 0;
}

