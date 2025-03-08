#ifndef __DENTON_MM_PGA_H
#define __DENTON_MM_PGA_H

#include <denton/compiler.h>
#include <denton/types.h>
#include <denton/mm/mm_types.h>

#include <stdlib.h>


struct page {
	atomic_t use_count;
	page_frame_t page_number;

	/* log2 of the number of pages allocated with this one */
	int order;

	unsigned long flags;

	struct list_head list_node;
	virtaddr_t virt;
};

enum page_flag {
	PAGE_INVALID = 31,
};

struct allocator;

/**
 * struct pga_ops - Page Allocator operations
 *
 * This structure describes operations needed to
 * manage physical pages.
 */
struct pga_ops {
	/**
	 * setup() - initialize the page allocator
	 * @ctx: token representing the allocator implementation
	 * @allocator: the bootstrap memory allocator for memory needed by the PGA
	 * @regions: list of memory regions available to by allocated
	 * @num_regions: count of @regions
	 *
	 * Returns 0 on success or a negative error code on failure.
	 */
	int (*setup)(void * ctx, const struct allocator* ally, const memory_t * regions, size_t num_regions);

	/**
	 * alloc() - allocate a (TODO: physically contiguous? bad constraint) number of pages
	 * @ctx: token representing the allocator implementation
	 * @order: the number of pages required. rounded up to the nearest power of 2
	 * @flags: page allocation flags
	 */
	struct page * (*alloc)(void * ctx, unsigned int order, pgf_t flags);

	/**
	 * free() - free a number of pages
	 */
	void (*free)(void * ctx, struct page * pagebase, unsigned int order);

	/**
	 * lookup() - get a pointer to page data, if it is a valid page
	 */
	struct page * (*lookup)(void * ctx, page_frame_t page_frame);
};

/**
 * abstract page allocator
 * @ctx: allocator-specific token for @ops
 * @ops: allocator operations
 * */
struct pga {
	void* ctx;
	const struct pga_ops* ops;
};

int           page_alloc_init(const struct allocator* bootally, memory_t * regions, size_t num_regions);
struct page * page_alloc(int order, unsigned int flags);
void          page_free(struct page * pg, size_t count);
struct page * page_lookup(page_frame_t page_frame);

page_frame_t  page_frame_from_physaddr(physaddr_t phys);
physaddr_t    page_to_phys(struct page * pg);
struct page * page_from_phys(physaddr_t phys);
struct page * page_from_virt(virtaddr_t virt);

#endif
