#ifndef __LIBC_ARCH_I386_ATOMIC_H
#define __LIBC_ARCH_I386_ATOMIC_H

#include <sys/compiler.h>
// #include <arch/rwonce.h>
#include "cmpxchg.h"
#include "rwmcc.h"
#include "rwonce.h"

#include <stdbool.h>

typedef struct {
	int counter;
} atomic_t;

/*
 * Atomic operations that C can't guarantee us.  Useful for
 * resource counting etc..
 */

static __always_inline int arch_atomic_read(const atomic_t *v)
{
	/*
	 * Note for KASAN: we deliberately don't use READ_ONCE_NOCHECK() here,
	 * it's non-inlined function that increases binary size and stack usage.
	 */
	return __READ_ONCE((v)->counter);
}

static __always_inline void arch_atomic_set(atomic_t *v, int i)
{
	__WRITE_ONCE(v->counter, i);
}

static __always_inline void arch_atomic_add(int i, atomic_t *v)
{
	asm volatile("lock; addl %1,%0"
		     : "+m" (v->counter)
		     : "ir" (i) : "memory");
}

static __always_inline void arch_atomic_sub(int i, atomic_t *v)
{
	asm volatile("lock; subl %1,%0"
		     : "+m" (v->counter)
		     : "ir" (i) : "memory");
}

static __always_inline bool arch_atomic_sub_and_test(int i, atomic_t *v)
{
	return GEN_BINARY_RMWcc("lock; subl", v->counter, e, "er", i);
}

static __always_inline void arch_atomic_inc(atomic_t *v)
{
	asm volatile("lock; incl %0"
		     : "+m" (v->counter) :: "memory");
}

static __always_inline void arch_atomic_dec(atomic_t *v)
{
	asm volatile("lock; decl %0"
		     : "+m" (v->counter) :: "memory");
}

static __always_inline bool arch_atomic_dec_and_test(atomic_t *v)
{
	return GEN_UNARY_RMWcc("lock; decl", v->counter, e);
}

static __always_inline bool arch_atomic_inc_and_test(atomic_t *v)
{
	return GEN_UNARY_RMWcc("lock; incl", v->counter, e);
}

static __always_inline bool arch_atomic_add_negative(int i, atomic_t *v)
{
	return GEN_BINARY_RMWcc("lock; addl", v->counter, s, "er", i);
}

static __always_inline int arch_atomic_add_return(int i, atomic_t *v)
{
	return i + xadd(&v->counter, i);
}

#define arch_atomic_sub_return(i, v) arch_atomic_add_return(-(i), v)

static __always_inline int arch_atomic_fetch_add(int i, atomic_t *v)
{
	return xadd(&v->counter, i);
}

#define arch_atomic_fetch_sub(i, v) arch_atomic_fetch_add(-(i), v)

static __always_inline int arch_atomic_cmpxchg(atomic_t *v, int old, int new)
{
	return arch_cmpxchg(&v->counter, old, new);
}

static __always_inline bool arch_atomic_try_cmpxchg(atomic_t *v, int *old, int new)
{
	return arch_try_cmpxchg(&v->counter, old, new);
}

static __always_inline int arch_atomic_xchg(atomic_t *v, int new)
{
	return arch_xchg(&v->counter, new);
}

static __always_inline void arch_atomic_and(int i, atomic_t *v)
{
	asm volatile("lock; andl %1,%0"
			: "+m" (v->counter)
			: "ir" (i)
			: "memory");
}

static __always_inline int arch_atomic_fetch_and(int i, atomic_t *v)
{
	int val = arch_atomic_read(v);

	do { } while (!arch_atomic_try_cmpxchg(v, &val, val & i));

	return val;
}

static __always_inline void arch_atomic_or(int i, atomic_t *v)
{
	asm volatile("lock; orl %1,%0"
			: "+m" (v->counter)
			: "ir" (i)
			: "memory");
}

static __always_inline int arch_atomic_fetch_or(int i, atomic_t *v)
{
	int val = arch_atomic_read(v);

	do { } while (!arch_atomic_try_cmpxchg(v, &val, val | i));

	return val;
}

static __always_inline void arch_atomic_xor(int i, atomic_t *v)
{
	asm volatile("lock; xorl %1,%0"
			: "+m" (v->counter)
			: "ir" (i)
			: "memory");
}

static __always_inline int arch_atomic_fetch_xor(int i, atomic_t *v)
{
	int val = arch_atomic_read(v);

	do { } while (!arch_atomic_try_cmpxchg(v, &val, val ^ i));

	return val;
}

#endif
