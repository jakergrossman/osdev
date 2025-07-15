#include "asm/cpu.h"
#include <asm/sync/spinlock.h>
#include <asm/irq.h>
#include <denton/spinlock.h>
#include <denton/compiler.h>

#include <stdatomic.h>

void spin_lock(spinlock_t* lock)
{
	for (;;) {
		if (likely(spin_trylock(lock))) {
			break;
		}

		cpu_relax();
	}
}

void spin_lock_irqsave(spinlock_t * lock, irq_flags_t * flags_out)
{
	irq_flags_t flags;
	for (;;) {
		flags = irq_save();
		if (likely(spin_trylock(lock))) {
			break;
		}
		irq_restore(flags);

		cpu_relax();
	}
	*flags_out = flags;
}

int spin_trylock(spinlock_t* lock)
{
	uint32_t expected = 0;
	return atomic_compare_exchange_strong_explicit(
		&lock->state,
		&expected,
		1,
		memory_order_acquire,
		memory_order_relaxed
	);
}

int spin_trylock_irqsave(spinlock_t* lock, irq_flags_t* flags);

void spin_unlock(spinlock_t* lock)
{
	uint32_t reset = 0;
	atomic_store_explicit(&lock->state, reset, memory_order_release);
}

void spin_unlock_restore(spinlock_t* lock, irq_flags_t flags)
{
	spin_unlock(lock);
	irq_restore(flags);
}

void spin_lock_irq(spinlock_t* lock)
{
	spin_unlock(lock);
	irq_disable();
}

void spin_unlock_irq(spinlock_t* lock)
{
	spin_unlock(lock);
	irq_enable();
}
