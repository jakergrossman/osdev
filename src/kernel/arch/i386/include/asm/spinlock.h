#include "asm/instr.h"
#include "asm/irq.h"
#include "denton/atomic.h"
#include "denton/compiler.h"
#include "denton/stddef.h"
#include <stdint.h>

typedef struct spinlock {
	uint32_t locked;
} spinlock_t;

#define SPINLOCK_INIT(n) { .locked = !!(n) }
#define SPIN_LOCK(name, n) \
	spinlock_t name = SPINLOCK_INIT(n)

static __always_inline void
spin_lock(spinlock_t* lock)
{
	while (!__sync_bool_compare_and_swap(&lock->locked, 0, 1))
		pause();
}

static __always_inline void
spin_unlock(spinlock_t* lock)
{
	__sync_synchronize();
	lock->locked = 0;
}

static inline void
spin_unlock_cleanup(spinlock_t** lock)
{
	spin_unlock(*lock);
}

static inline void
spin_lock_cleanup(spinlock_t** lock)
{
	spin_lock(*lock);
}

/* Wraps acquiring and releaseing a spinlock. Usages of 'using_spinlock' can't
 * ever leave-out a matching release for the acquire. */
#define using_spin_lock(lockp) scoped_using_cond(1, spin_lock, spin_unlock_cleanup, lockp)

/* Can be used in a 'using_spinlock' block of code to release a lock for a
 * section of code, and then acquire it back after that code is done.
 *
 * It's useful in sections of code where we may go to sleep, and we want to
 * release the lock before yielding, and then acquire the lock back when we
 * start running again. */
#define not_using_spin_lock(lockp) scoped_using_cond(1, spin_lock, spin_lock_cleanup, lockp)
