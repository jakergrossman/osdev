#include <denton/mm/pga.h>
#include <denton/mm/bootmem.h>
#include <denton/atomic.h>
#include <denton/types.h>
#include <denton/klog.h>
#include <denton/bits.h>
#include "denton/bma.h"
#include <denton/spinlock.h>

#include <asm/bitops.h>
#include <asm/findbit.h>
#include <asm/paging.h>

#include <string.h>
#include <stdlib.h>

struct {
	struct page * pages;
	size_t num_pages;
	unsigned long * free_bitmask;
	size_t num_bits;
	spinlock_t lock;
} ally = {
	.lock = SPINLOCK_INIT(0),
};

static void
__page_mark_free(pfn_t pfn)
{
	clr_bit(pfn, ally.free_bitmask);
}

void page_alloc_init(size_t pages, memory_t* memregions, size_t num_regions)
{
	klog_debug("Initializing page allocator\n");

	ally.num_pages = pages;
	ally.pages = bootmem_alloc(pages * sizeof(*ally.pages), PAGE_SIZE);

	/* lowest bits required that is aligned to the word size */
	ally.num_bits = ALIGN_2(pages, __BITS_PER_LONG);
	ally.free_bitmask = bootmem_alloc(ally.num_bits / 8, PAGE_SIZE);

	/* initially:
	 *  - all pages are zeroed and claimed
	 *  - all pages are set invalid
	 */
	memset(ally.pages, 0, pages * sizeof(*ally.pages));
	memset(ally.free_bitmask, ~0, ally.num_bits * 8);

	for (size_t page = 0; page < pages; page++) {
		struct page* p = &ally.pages[page];
		p->page_number = page;
		p->virt = p_to_v((p->page_number) << PAGE_SHIFT);
		set_bit(PAGE_INVALID, &p->flags);
	}

	for (int i = 0; i < num_regions; i++) {
		if (!memregions[i].start) {
			continue;
		}

		physaddr_t start = PAGE_ALIGN(memregions[i].start);
		physaddr_t end = PAGE_ALIGN_DOWN(memregions[i].end_ex);

		klog_debug("Adding range [%u,%u] to allocator\n", start / PAGE_SIZE, end / PAGE_SIZE);
		for (physaddr_t loc = start; loc < end; loc += PAGE_SIZE) {
			struct page* p = page_from_phys(loc);

			/* all unused bootmem marked valid and available */
			clr_bit(PAGE_INVALID, &p->flags);
			bma_free(ally.free_bitmask, pfn_from_physaddr(loc), 1);
		}
	}
}

struct page* page_from_pfn(pfn_t pfn)
{
	return ally.pages + pfn;
}

/* for when you know what you want */
static struct page*
__page_alloc(unsigned long pos, int count, unsigned int flags)
{
	struct page* page_base = &ally.pages[pos];
	for (size_t i = 0; i < count; i++) {
		struct page* page = &page_base[i];
		set_bit(pos+i, ally.free_bitmask);
		page_get(page);
	}

	return page_base;
}

struct page*
page_alloc(int count, unsigned int flags)
{
	using_spin_lock(&ally.lock) {
		long base = bma_alloc_consecutive(ally.free_bitmask, ally.num_bits, count);
		if (base >= 0) {
			return __page_alloc(base, count, flags);
		}
	}

	return NULL;
}

void page_free(struct page* start, size_t count)
{
	using_spin_lock(&ally.lock) {
		bma_free(ally.free_bitmask, start->page_number, count);
	}
}

void page_get(struct page* page)
{
	atomic_inc(&page->use_count);
}

void page_put(struct page* page)
{
	if (atomic_dec_and_test(&page->use_count)) {
		__page_mark_free(page->page_number);
	}
}
