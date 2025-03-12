#include "asm/instr.h"
#include "asm/irq.h"
#include "asm/rwonce.h"
#include "denton/compiler.h"
#include "denton/stddef.h"
#include <stdint.h>

#include "sync/spin.h"

#include <stdatomic.h>

// typedef struct {
// 	uint32_t locked;
// } spinlock_t;

// // #define SPINLOCK_INIT(n) { .next_ticket = (n), .now_serving = (n) }
// #define SPINLOCK_INIT(n) { 0 }
// #define SPIN_LOCK(name, n) \
// 	spinlock_t name = SPINLOCK_INIT(n)

// static inline int
// spin_trylock(spinlock_t* lock, irq_flags_t* flags)
// {
// }

// static inline irq_flags_t
// spin_lock_irqsave(spinlock_t * lock, irq_flags_t * flags)
// {
// 	unsigned long ret;
// 	for (;;) {
// 		int val = atomic_load(
// 		uint32_t old = 0;

// 		if (!val && try_cmpxchg_acquire(&lock->locked, &old, 1)) {
// 			if (!val) {
// 				break;
// 			}
// 		}
// 		pause();
// 	}
// }

// static __always_inline void
// spin_lock(spinlock_t* lock)
// {
// 	if (!__sync_bool_compare_and_swap(&lock->locked, 0, 1)) {
// 		cli();
// 	}
// 	cli();
// 	while (!__sync_bool_compare_and_swap(&lock->locked, 0, 1)) {
// 		pause();
// 		cli();
// 	}
// }

// static __always_inline void
// spin_unlock(spinlock_t* lock)
// {
// 	__sync_synchronize();
// 	lock->locked = 0;
// }

// void spin_lock(spinlock_t* lock);
// void spin_unlock(spinlock_t* lock);

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

/* Wraps acquiring and releaseing a spin_lock. Usages of 'using_spin_lock' can't
 * ever leave-out a matching release for the acquire. */
#define using_spin_lock(lockp) scoped_using_cond(1, spin_lock, spin_unlock_cleanup, lockp)

/* Can be used in a 'using_spin_lock' block of code to release a lock for a
 * section of code, and then acquire it back after that code is done.
 *
 * It's useful in sections of code where we may go to sleep, and we want to
 * release the lock before yielding, and then acquire the lock back when we
 * start running again. */
#define not_using_spin_lock(lockp) scoped_using_cond(1, spin_lock, spin_lock_cleanup, lockp)
