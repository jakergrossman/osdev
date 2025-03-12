/* ticket spinlock */

#include <denton/spinlock.h>
#include <asm/instr.h>

#include <stdint.h>

void spin_lock(spinlock_t* lock)
{
	/* relaxed ordering. we dont care who gets what, so long as
	 * everyone gets a unique incrementing number eventually
	 */
	// uint32_t my_ticket = __atomic_fetch_add(&lock->next_ticket, 1, __ATOMIC_RELAXED);
	// while (my_ticket != __atomic_load_n(&lock->now_serving, __ATOMIC_ACQUIRE)) {
	// 	pause();
	// }
}

void spin_unlock(spinlock_t* lock)
{
	// uint32_t me = __atomic_load_n(&lock->now_serving, __ATOMIC_RELAXED);
	// (void)__atomic_store_n(&lock->now_serving, me+1, __ATOMIC_RELEASE);
}
