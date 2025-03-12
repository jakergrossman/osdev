#ifndef __DENTON_ARCH_I386_SYNC_SPIN_H
#define __DENTON_ARCH_I386_SYNC_SPIN_H

#include <asm/sync/tspinlock.h>

typedef tspinlock_t spinlock_t;

#define SPINLOCK_INIT(n) TSPINLOCK_INIT(n)
#define SPIN_LOCK(name, n) TSPIN_LOCK(name, n)
#define spin_lock tspin_lock
#define spin_trylock tspin_trylock
#define spin_unlock tspin_unlock
#define spin_lock_irqsave tspin_lock_irqsave
#define spin_unlock_irqrestore tspin_unlock_irqrestore

#endif
