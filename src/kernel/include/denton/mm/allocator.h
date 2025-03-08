#ifndef __DENTON_MM_ALLOCATOR_H
#define __DENTON_MM_ALLOCATOR_H

#include <denton/mm/mm_types.h>
#include <stddef.h>

struct allocator_ops {
	void* (*alloc)(void* ctx, size_t len, unsigned int flags);
	void  (*free)(void* ctx, void* memory);
};

struct allocator {
	void* ptr;
	const struct allocator_ops * const ops;
};

/**
 * kmem_alloc_ally(allocate 
 */
void* kmem_alloc_ally(const struct allocator* ally, size_t len, pgf_t flags);
void  kmem_free_ally(const struct allocator* ally, void* memory);

#endif
