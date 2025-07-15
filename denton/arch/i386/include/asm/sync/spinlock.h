#ifndef __DENTON_ARCH_I386_SYNC_SPIN_H
#define __DENTON_ARCH_I386_SYNC_SPIN_H

#include <stdint.h>

struct arch_spinlock_s {
	_Atomic(uint32_t) state;
};

typedef struct arch_spinlock_s spinlock_t;

#define SPINLOCK_INIT(n) { .state = (n) }
#define SPIN_LOCK(name, n) \
	spinlock_t name = SPINLOCK_INIT(n)

#endif
