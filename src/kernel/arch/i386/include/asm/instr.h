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

static __always_inline void
outb(uint16_t port, uint8_t value)
{
	asm volatile (
		"outb %1, %0"
		:
		: "dN" (port), "a" (value)
	);
}

static __always_inline uint8_t
inb(uint16_t port)
{
	uint8_t ret;
	asm volatile (
		"inb %1, %0"
		: "=a" (ret)
		: "dN" (port)
	);
	return ret;
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


#endif
