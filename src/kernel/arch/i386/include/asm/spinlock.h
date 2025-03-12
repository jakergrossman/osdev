#include "asm/instr.h"
#include "asm/irq.h"
#include "asm/rwonce.h"
#include "denton/compiler.h"
#include "denton/stddef.h"
#include <stdint.h>

// #include "sync/spin.h"

#include <stdatomic.h>

typedef arch_spinlock_t spinlock_t;

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
