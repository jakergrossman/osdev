#ifndef __DENTON_MM_BOOTMEM_H
#define __DENTON_MM_BOOTMEM_H

#include "denton/compiler.h"
#include <denton/types.h>
#include <denton/math.h>

#include <stddef.h>
#include <stdint.h>

/* add a memory region for use by boot allocator */
int bootmem_add(uint64_t start, uint64_t end);

void* bootmem_alloc(size_t length, size_t alignment);

/* init page allocator. no more bootmem after call! */
void bootmem_setup_pga(void);

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




#endif
