#ifndef __ARCH_I386_ASM_ATOMIC_H
#define __ARCH_I386_ASM_ATOMIC_H

#include <denton/compiler.h>

#include <stdbool.h>
#include <stdint.h>

typedef struct {
	int32_t count;
} __align(4) atomic32_t;

#define ATOMIC32_INIT(i) { (i) }

static __always_inline int32_t
atomic32_get(const atomic32_t* v)
{
	return (*(volatile int32_t *)&(v)->count);
}

static __always_inline void
atomic32_set(atomic32_t* v, int32_t count)
{
	*(volatile int32_t*)&v->count = count;
}

static __always_inline void
atomic32_add(atomic32_t* v, int32_t count)
{
	asm volatile(
		"lock; addl %1, %0"
		: "+m" (v->count)
		: "ir" (count)
	);
}

static __always_inline void
atomic32_sub(atomic32_t* v, int32_t count)
{
	asm volatile(
		"lock; subl %1, %0"
		: "+m" (v->count)
		: "ir" (count)
	);
}


static __always_inline int32_t
atomic32_xadd(atomic32_t* v, int32_t count)
{
	asm volatile(
		"lock; xaddl %1, %0"
		: "+m" (v->count), "+r" (count)
	);
	return count;
}

static __always_inline int32_t
atomic32_sub_return(atomic32_t* v, int32_t count)
{
	return atomic32_xadd(v, -count) - count;
}

static __always_inline int32_t
atomic32_add_return(atomic32_t* v, int32_t count)
{
	return atomic32_xadd(v, count);
}

static __always_inline void
atomic32_inc(atomic32_t* v)
{
	asm volatile( "lock; incl %0" : "+m" (v->count));
}

static __always_inline void
atomic32_dec(atomic32_t* v)
{
	asm volatile( "lock; decl %0" : "+m" (v->count));
}


static __always_inline int32_t
atomic32_inc_return(atomic32_t* v)
{
	return atomic32_add_return(v, 1);
}

static __always_inline int32_t
atomic32_dec_return(atomic32_t* v)
{
	return atomic32_add_return(v, -1);
}

static __always_inline bool
atomic32_dec_and_test(atomic32_t* v)
{
	asm volatile goto (
		"lock; decl %0\n\t"
		"jz %l[equal]"
		:
		: "m" (v->count)
		: "memory"
		: equal
	);
	return false;
equal:
	return true;
}


#endif
