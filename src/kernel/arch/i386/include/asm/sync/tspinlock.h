#ifndef __TSPINLOCK_H
#define __TSPINLOCK_H

#include "asm/instr.h"
#include <stdatomic.h>
#include <stdbool.h>

#define TSPINLOCK_INIT(n) { .next_ticket = (n), .next_entrance = (n) }
#define TSPIN_LOCK(name, n) \
	tspinlock_t name = TSPINLOCK_INIT(n)

/**
 * ticket-lock
 */
typedef union tspinlock_s {
	_Atomic(uint32_t) ticket_state;
	struct {
		_Atomic(uint16_t) next_ticket;
		_Atomic(uint16_t) next_entrance;
	};
} tspinlock_t;

void tspin_lock(tspinlock_t* lock);
bool tspin_trylock(tspinlock_t* lock);
void tspin_unlock(tspinlock_t* lock);


#endif

// namespace /*Spinlock*/ Kernel
// {
// 	FIL void tSpinlock::Init()
// 	{
// 		LockTicket = 2;
// 		LockUserTicket = 2;
// 	}

// 	FIL QWORD tSpinlock::NumberOfWaitingLocksRequested() // Not guaranteed to return the correct answer, just used as a hint
// 	{
// 		return LockTicket - LockUserTicket;
// 	}

// 	FIL QWORD tSpinlock::TryLock(QWORD& LockTicketToWaitFor) // Pass through 1 for the first lock
// 	{
// 		if (LockTicketToWaitFor == 1)
// 			LockTicketToWaitFor = __sync_fetch_and_add(&LockTicket, 1);

// 		return LockTicketToWaitFor - LockUserTicket; // Return the number of locks before this one, 0 = Lock Acquired
// 	}

// 	FIL void tSpinlock::Lock()
// 	{
// 		QWORD LockTicketToWaitFor = __sync_fetch_and_add(&LockTicket, 1);

// 		while (LockTicketToWaitFor != LockUserTicket)
// 		{
// 			// Util.Pause(); // On my servers it runs faster without Pause
// 		}
// 	}

// 	FIL void tSpinlock::Unlock()
// 	{
// 		LockUserTicket += 1;
// 	}
// }
