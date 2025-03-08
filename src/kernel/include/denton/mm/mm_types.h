#ifndef __DENTON_MM_TYPES_H
#define __DENTON_MM_TYPES_H

#include <denton/types.h>
#include <denton/bits.h>
#include <stddef.h>

typedef struct memory {
	physaddr_t start;
	physaddr_t end_ex;
} memory_t ;

static __always_inline size_t
memory_size(memory_t mem)
{
	return mem.end_ex - mem.start;
}

static __always_inline size_t
memory_intersects(memory_t bounds, memory_t obj)
{
	return (bounds.start <= obj.end_ex) && (obj.start <= bounds.end_ex);
}

static __always_inline size_t
memory_contains(memory_t bounds, memory_t obj)
{
	return (bounds.start <= obj.start) && (obj.end_ex <= bounds.end_ex);
}

typedef uint32_t page_frame_t;

/** "page get flags" */
typedef enum pgf_e {
#define __PGF_ALIGN_MASK      __GENMASK(4, 0)
#define \
	PGF_ALIGN(n) ((n) & __PGF_ALIGN_MASK)
	PGF_KERNEL = BIT(5),
	PGF_USER   = BIT(6),
} pgf_t;

static inline size_t pgf_align(pgf_t flags)
{
	return (1UL << (__PGF_ALIGN_MASK & flags));
}

#endif
