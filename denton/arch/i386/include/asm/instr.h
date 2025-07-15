#ifndef __DENTON_ARCH_I386_ASM_HALT_H
#define __DENTON_ARCH_I386_ASM_HALT_H

#include <denton/compiler.h>
#include <denton/types.h>
#include <stdint.h>
#include <sys/cdefs.h>

static __always_inline void cli(void)
{
	asm volatile ( "cli" );
}

static __always_inline void sti(void)
{
	asm volatile ( "sti" );
}

static __always_inline void interrupt(uint8_t irqno)
{
	asm volatile ( "irq $0" : : "r" (irqno) );
}

static __always_inline void hlt(void)
{
	asm volatile ( "hlt" );
}

static __always_inline uint32_t
eflags_read(void)
{
	uint32_t flags;
	asm volatile(
		"pushfl\n\t"
		"popl %0\n"
		: "=a" (flags)
	);
	return flags;
}

static __always_inline void
eflags_write(uint32_t flags)
{
	asm volatile(
		"pushfl\n\t"
		"popl %0\n"
		:
		: "a" (flags)
	);
}

static __always_inline uint32_t
xchg(virtaddr_t addr, uint32_t val)
{
	volatile uint32_t* ptr = addr;
	uint32_t result;

	asm volatile(
		"lock; xchgl %0, %1"
		: "+m" (*ptr), "=a" (result)
		: "1" (val)
		: "cc", "memory"
	);
	return result;
}

static __always_inline void
pause(void)
{
	asm volatile ( "pause" );
}

#define __raw_cmpxchg(ptr, old, new, sizeof_ptr) ({ \
	__typeof__(*(ptr)) __ret; \
	__typeof__(*(ptr)) __old = (old); \
	__typeof__(*(ptr)) __new = (new); \
	switch (sizeof_ptr) { \
	case sizeof(uint8_t): \
	{ \
		volatile uint8_t *__ptr = (volatile uint8_t*)(ptr); \
		asm volatile( \
			"lock; cmpxchgb %2, %1" \
			: "=a" (__ret), "+m" (*__ptr) \
			: "q" (__new), "0" (__old) \
			: "memory" \
		); \
		break; \
	} \
	case sizeof(uint16_t): \
	{ \
		volatile uint16_t *__ptr = (volatile uint16_t*)(ptr); \
		asm volatile( \
			"lock; cmpxchw %2, %1" \
			: "=a" (__ret), "+m" (*__ptr) \
			: "q" (__new), "0" (__old) \
			: "memory" \
		); \
		break; \
	} \
	case sizeof(uint32_t): \
	{ \
		volatile uint32_t *__ptr = (volatile uint32_t*)(ptr); \
		asm volatile( \
			"lock; cmpxchgl %2, %1" \
			: "=a" (__ret), "+m" (*__ptr) \
			: "q" (__new), "0" (__old) \
			: "memory" \
		); \
		break; \
	} \
	case sizeof(uint64_t): \
	{ \
		volatile uint64_t *__ptr = (volatile uint64_t*)(ptr); \
		asm volatile( \
			"lock; cmpxchgq %2, %1" \
			: "=a" (__ret), "+m" (*__ptr) \
			: "q" (__new), "0" (__old) \
			: "memory" \
		); \
		break; \
	} \
	} \
	__ret; \
})

#define try_cmpxchg_acquire(__ptr, __oldp, __new) \
({ \
	typeof(*(__ptr)) *__op = (__oldp), __o = *__op, __r; \
	__r = __raw_cmpxchg((__ptr), __o, (__new), sizeof(*(__ptr))); \
	if (unlikely(__r != __o)) { \
		*__op = __r; \
	} \
	likely(__r == __o); \
})

#endif
