#ifndef __ARCH_I386_BUG_H
#define __ARCH_I386_BUG_H

#include "denton/types.h"
#include <denton/compiler.h>


/* a place for all the bugs to live, so they don't infest our program :) */

#define BUILD_BUG_ON_ZERO(expr) \
	((int)(sizeof(struct { int: (-!!(expr)); })))

/* permits the compiler to check the validity of the expression
 * without code generation or side effects */
#define BUILD_BUG_ON_INVALID(expr) \
	((void)(sizeof((__force long)))

/**
 * struct bug_entry - details about a kernel bug
 * @addr: The address of the instruction that caused the bug
 * @why:  Optional textual description of why this is a bug, or NULL
 * @file: File the bug occurred in
 * @line: Line the bug occurred on
 * @flags: flags for determining between WARN, BUG, etc.
 */
struct bug_entry {
	uintptr_t addr;
	const char* why;
	const char* file;
	unsigned long line;
	unsigned long flags;
};

extern struct bug_entry __BUG_TABLE_START[], __BUG_TABLE_END[];

/* place a bug_entry in the __bug_table section */
#define __BUG(instr, flags, why)                   \
	asm_inline volatile (                          \
		"1:" instr "\n"                            \
		"   .pushsection __bug_table, \"aw\"\n"    \
		"2: .long 1b  # struct bug_entry::addr\n"  \
		"   .long %c0 # struct bug_entry::why\n"   \
		"   .long %c1 # struct bug_entry::file\n"  \
		"   .word %c2 # struct bug_entry::line\n"  \
		"   .word %c3 # struct bug_entry::flags\n" \
		"   .org 2b+%c4\n"                       \
		"   .popsection\n"                         \
		: : "i" (why),                        \
		    "i" (__FILE__),                        \
		    "i" (__LINE__),                        \
		    "i" (flags),                           \
		    "i" (sizeof(struct bug_entry))         \
	)

/**
 * BUG() - indicate that reaching this point is an unrecoverable error
 * @...: if present, a textual description of why this is a bug.
 *
 * Emits an invalid instruction to force an exception, and records
 * a corresponding entry in __bug_table
 */
#define BUG(...) \
do { \
	__BUG("UD2", 0, VALUE_IFNOT(__VA_OPT__(1), NULL) __VA_ARGS__); \
	__builtin_unreachable(); \
} while (0)

static inline bool
bug_is_ud2(uint32_t addr)
{
	const uint8_t ud2_bytes[sizeof(uint16_t)] = { 0x0F, 0x0B };
	uint8_t* ptr = (uint8_t*)addr;
	for (size_t i = 0; i < sizeof(ud2_bytes); i++) {
		if (ptr[i] != ud2_bytes[i]) {
			return false;
		}
	}
	return true;
}

static inline struct bug_entry* bug_find(uintptr_t bugaddr)
{
	if (!bug_is_ud2(bugaddr))
	{
		return NULL;
	}

	struct bug_entry* bug;
	for (bug = __BUG_TABLE_START; bug < __BUG_TABLE_END; bug++) {
		if (bugaddr == bug->addr) {
			return bug;
		}
	}

	return NULL;
}

#endif
