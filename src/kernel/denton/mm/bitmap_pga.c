#include <denton/mm/bitmap_pga.h>

#include <denton/mm/pga.h>
#include <denton/atomic.h>
#include <denton/types.h>
#include <denton/bits.h>
#include <denton/bma.h>
#include <denton/spinlock.h>
#include <denton/errno.h>
#include <denton/mm/allocator.h>

#include <asm/memlayout.h>
#include <asm/bitops.h>
#include <asm/findbit.h>
#include <asm/paging.h>

#include <string.h>
#include <stdlib.h>

static struct bitmap_ally {
	struct page * pages;
	size_t num_pages;
	unsigned long * free_bitmask;
	size_t num_bits;
	spinlock_t lock;
} bma_ally = {
	.lock = SPINLOCK_INIT(0),
};

int bma_page_alloc_init(
	void* ctx,
	const struct allocator* bootally,
	const memory_t* memregions,
	size_t num_regions
) {
	if (!memregions || !num_regions || !ctx) {
		return -EINVAL;
	}

	struct bitmap_ally* ally = ctx;


	page_frame_t highest_page = 0;
	for (size_t i = 0; i < num_regions; i++) {
		physaddr_t end_ex = PAGE_ALIGN_DOWN(memregions[i].end_ex);
		page_frame_t last_page = page_frame_from_physaddr(end_ex);
		highest_page = kmax(highest_page, last_page);
	}

	ally->num_pages = highest_page;
	ally->pages = kmem_alloc_ally(bootally, highest_page * sizeof(*ally->pages), PAGE_SIZE);

	/* lowest bits required that is aligned to the word size */
	ally->num_bits = ALIGN_2(highest_page, __BITS_PER_LONG);
	ally->free_bitmask = kmem_alloc_ally(bootally, ally->num_bits / 8, PAGE_SIZE);

	/* initially:
	 *  - all pages are zeroed and claimed
	 *  - all pages are set invalid
	 */
	memset(ally->pages, 0, highest_page * sizeof(*ally->pages));
	memset(ally->free_bitmask, ~0, ally->num_bits * 8);

	for (size_t page = 0; page < highest_page; page++) {
		struct page* p = &ally->pages[page];
		p->page_number = page;
		p->virt = p_to_v((p->page_number) << PAGE_SHIFT);
		set_bit(PAGE_INVALID, &p->flags);
	}

	int total_pages = 0;
	for (int i = 0; i < num_regions; i++) {
		if (!memregions[i].start) {
			continue;
		}

		physaddr_t start = PAGE_ALIGN(memregions[i].start);
		physaddr_t end = PAGE_ALIGN_DOWN(memregions[i].end_ex);

		for (physaddr_t loc = start; loc < end; loc += PAGE_SIZE) {
			struct page* p = page_from_phys(loc);

			/* all unused bootmem marked valid and available */
			clr_bit(PAGE_INVALID, &p->flags);
			bma_free(ally->free_bitmask, page_frame_from_physaddr(loc), 1);
			total_pages++;
		}
	}


	return total_pages;
}

struct page*
bma_page_from_page_frame(void* ctx, page_frame_t page_frame)
{
	struct bitmap_ally* ally = ctx;
	return ally->pages + page_frame;
}

/* for when you know what you want */
static struct page*
__bma_page_alloc(struct bitmap_ally* ally, unsigned long pos, int count, unsigned int flags)
{
	struct page* page_base = &ally->pages[pos];
	for (size_t i = 0; i < count; i++) {
		set_bit(pos+i, ally->free_bitmask);
	}

	return page_base;
}

struct page*
bma_page_alloc(struct bitmap_ally* ally, int count, unsigned int flags)
{
	using_spin_lock(&ally->lock) {
		long base = bma_alloc_consecutive(ally->free_bitmask, ally->num_bits, count);
		if (base >= 0) {
			return __bma_page_alloc(ally, base, count, flags);
		}
	}

	return NULL;
}

void bma_page_free(struct bitmap_ally* ally, struct page* start, size_t count)
{
	using_spin_lock(&ally->lock) {
		bma_free(ally->free_bitmask, start->page_number, count);
	}
}

static struct page* bma_pga_alloc(void* ctx, unsigned int order, pgf_t flags)
{
	return bma_page_alloc(ctx, order, 0);
}

static void bma_pga_free(void* ctx, struct page* pagebase, unsigned int order)
{
	return bma_page_free(ctx, pagebase, 0);
}

static const struct pga_ops bma_pga_ops = {
	.setup = bma_page_alloc_init,
	.alloc = bma_pga_alloc,
	.free = bma_pga_free,
	.lookup = bma_page_from_page_frame,
};

const struct pga bitmap_pga = {
	.ctx = &bma_ally,
	.ops = &bma_pga_ops,
};
