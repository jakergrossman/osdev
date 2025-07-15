#ifndef __ARCH_I386_ASM_IO_H
#define __ARCH_I386_ASM_IO_H

#include <denton/types.h>

#include <stdint.h>

#define DECL_IO(__bwl, __bw, __width) \
static __always_inline void out##__width (uint##__width##_t value, uint16_t port) { \
	asm volatile ( \
		"out" #__bwl " %" #__bw "0, %w1" \
		:: "a"(value), "Nd"(port) \
	); \
} \
/** foo */ \
static __always_inline uint##__width##_t in##__width (uint16_t port)  { \
	uint##__width##_t __value; \
	asm volatile ( \
		"in" #__bwl " %w1, %" #__bw "0" \
		: "=a"(__value) : "Nd"(port) \
	); \
	return __value; \
}

DECL_IO(b, b, 8);
DECL_IO(w, w, 16);
DECL_IO(l,  , 32);
#undef DECL_IO

#endif
