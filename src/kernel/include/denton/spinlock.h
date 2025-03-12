#ifndef __DENTON_SPINLOCK_H
#define __DENTON_SPINLOCK_H

// #include "asm/cpu.h"
// #include <asm/spinlock.h>

#include <asm/irq.h>
#include <asm/sync/spinlock.h>


void spin_lock(spinlock_t* lock);
void spin_lock_irqsave(spinlock_t* lock, irq_flags_t* flags);

int  spin_trylock(spinlock_t* lock);
int  spin_trylock_irqsave(spinlock_t* lock, irq_flags_t* flags);

void spin_unlock(spinlock_t* lock);
void spin_unlock_restore(spinlock_t* lock, irq_flags_t flags);

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

#endif
