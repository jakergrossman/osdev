#ifndef __DENTON_ARCH_I386_ASM_BITOPS_H
#define __DENTON_ARCH_I386_ASM_BITOPS_H

#include <denton/bits.h>
#include <denton/math.h>
#include "rmwcc.h"

#include <asm/barrier.h>
#include <asm/asm.h>

#include <stdbool.h>
#include <stddef.h>

#define RLONG_ADDR(x)  "m" (*(volatile long *)(x))
#define WBYTE_ADDR(x) "+m" (*(volatile char *)(x))

#define CONST_MASK_ADDR(nr, addr) WBYTE_ADDR((void*)(addr) + ((nr) >> 3))
#define CONST_MASK(nr) (1 << ((nr) & 7))


/**
 * set_bit - Atomically set a bit in memory
 * @nr: bit index to set
 * @addr: address to start counting from
 */
static inline void
set_bit(long nr, volatile void* addr)
{
	if (__builtin_constant_p(nr)) {
		asm volatile("lock; orb %b1,%0"
			: CONST_MASK_ADDR(nr, addr)
			: "iq" (CONST_MASK(nr))
			: "memory");
	} else {
		asm volatile("lock; btsl %1,%0"
			: : RLONG_ADDR(addr), "Ir" (nr) : "memory");
	}
}

/**
 * clr_bit - Atomically clear a bit in memory
 * @nr: bit index to clear
 * @addr: address to start counting from
 */
static inline void
clr_bit(long nr, volatile void* addr)
{
	if (__builtin_constant_p(nr)) {
		asm volatile("lock; andb %b1,%0"
			: CONST_MASK_ADDR(nr, addr)
			: "iq" (CONST_MASK(nr))
			: "memory");
	} else {
		asm volatile("lock; btrl %1,%0"
			: : RLONG_ADDR(addr), "Ir" (nr) : "memory");
	}
}

/**
 * chg_bit - Atomically change a bit in memory
 * @nr: bit index to change
 * @addr: address to start counting from
 */
static inline void
chg_bit(unsigned long nr, volatile void* addr)
{
	if (__builtin_constant_p(nr)) {
		asm volatile("lock; xorb %b1,%0"
			: CONST_MASK_ADDR(nr, addr)
			: "iq" (CONST_MASK(nr))
			: "memory");
	} else {
		asm volatile("lock; btcl %1,%0"
			: : RLONG_ADDR(addr), "Ir" (nr) : "memory");
	}
}

static inline bool
test_and_set_bit(long nr, volatile unsigned long* addr)
{
	return GEN_BINARY_RMWcc("lock; btsl", *addr, c, "Ir", nr);
}

static inline bool
test_and_clr_bit(long nr, volatile unsigned long* addr)
{
	return GEN_BINARY_RMWcc("lock; btrl", *addr, c, "Ir", nr);
}

static inline bool
test_and_chg_bit(long nr, volatile unsigned long* addr)
{
	return GEN_BINARY_RMWcc("lock; btcl", *addr, c, "Ir", nr);
}


/* like set_bit(), but without the atomic guarantees */
static inline void
__set_bit(unsigned long nr, volatile unsigned long* addr)
{
	asm volatile("btsl %1,%0" : : RLONG_ADDR(addr), "Ir" (nr) : "memory");
}

/* like clr_bit(), but without the atomic guarantees */
static inline void
__clr_bit(unsigned long nr, volatile unsigned long* addr)
{
	asm volatile("btrl %1,%0" : : RLONG_ADDR(addr), "Ir" (nr) : "memory");
}

/* like chg_bit(), but without the atomic guarantees */
static inline void
__chg_bit(unsigned long nr, volatile unsigned long* addr)
{
	asm volatile("btcl %1,%0" : : RLONG_ADDR(addr), "Ir" (nr) : "memory");
}

static inline bool
__test_and_clr_bit(unsigned long nr, volatile unsigned long *addr)
{
	bool oldbit;

	asm volatile("btrl %2,%1"
		     CC_SET(c)
		     : CC_OUT(c) (oldbit)
		     : RLONG_ADDR(addr), "Ir" (nr) : "memory");
	return oldbit;
}

static inline unsigned long
__variable__ffs(unsigned long word)
{
	asm("rep; bsf %1,%0"
			: "=r" (word)
			: "rm" (word));
	return word;
}

/* find first set bit in word */
static inline unsigned long
__ffs(unsigned long word)
{
	return __builtin_constant_p(word)
			? (unsigned long)__builtin_ctzl(word)
			: __variable__ffs(word);
}

static inline unsigned long
__variable__ffz(unsigned long word)
{
	asm("rep; bsf %1,%0"
			: "=r" (word)
			: "rm" (~word));
	return word;
}

/* find first clear bit in word */
static inline unsigned long
__ffz(unsigned long word)
{
	return __builtin_constant_p(word)
			? (unsigned long)__builtin_ctzl(~word)
			: __variable__ffs(word);
}

/* find last set bit in word */
static inline unsigned long
__fls(unsigned long word)
{
	if (__builtin_constant_p(word)) {
		return __BITS_PER_LONG - 1 - __builtin_clzl(word);
	}

	asm("bsr %1,%0"
		: "=r" (word)
		: "rm" (word));
	return word;
}

#endif
