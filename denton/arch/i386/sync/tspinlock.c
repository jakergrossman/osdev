#include <asm/sync/tspinlock.h>

void tspin_lock(tspinlock_t* lock) {
	uint32_t myticket = atomic_fetch_add_explicit(&lock->next_ticket, 1, memory_order_relaxed);
	while (atomic_load_explicit(&lock->next_entrance, memory_order_acquire) != myticket) {
		pause();
	}
}

void tspin_unlock(tspinlock_t* lock)
{
	uint16_t me = atomic_load_explicit(&lock->next_entrance, memory_order_relaxed);
	atomic_store_explicit(&lock->next_entrance, me+1, memory_order_release);
}

bool tspin_trylock(tspinlock_t* lock) {
	bool locked = false;

	/* the current state of the lock */
	tspinlock_t oldlock;
	oldlock.ticket_state = atomic_load_explicit(&lock->ticket_state, memory_order_relaxed);
	/* the state of the lock if we successfully take it */
	tspinlock_t newlock = oldlock;
	newlock.next_ticket += 1;

	/* only try if there is even a chance */
	if (oldlock.next_ticket == oldlock.next_entrance) {
		if (atomic_compare_exchange_strong_explicit(
			&lock->ticket_state,
			(uint32_t*)&oldlock.ticket_state,
			(uint32_t)&newlock.ticket_state,
			memory_order_acquire,  /* success */
			memory_order_relaxed) /* fail */
		) {
			locked = true;
		}
	}

	return locked;
}
