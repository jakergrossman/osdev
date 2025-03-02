#ifndef __DENTON_MM_PGA_H
#define __DENTON_MM_PGA_H

#include "denton/mm/bootmem.h"
#include "denton/types.h"
#include <denton/compiler.h>
#include <denton/atomic.h>
#include <denton/list.h>

#include <asm/paging.h>
#include <asm/memlayout.h>

struct page {
	atomic_t use_count;
	pfn_t page_number;

	/* log2 of the number of pages allocated with this one */
	int order;

	unsigned long flags;

	struct list_head list_node;
	virtaddr_t virt;
};

enum page_flag {
	PAGE_INVALID = 31,
};

struct page* page_from_pfn(pfn_t pfn);

static __always_inline struct page*
page_from_phys(physaddr_t phys)
{
	return page_from_pfn(__physaddr_to_pfn(phys));
}

static __always_inline struct page*
page_from_virt(virtaddr_t virt)
{
	return page_from_pfn(V2P(virt));
}

static __always_inline physaddr_t
page_to_phys(struct page* pg)
{
	return (pg->page_number << PAGE_SHIFT);
}

void page_alloc_init(size_t pages, memory_t* regions, size_t num_regions);
struct page* page_alloc(int order, unsigned int flags);
void page_free(struct page* pg, size_t count);
void page_get(struct page* page);
void page_put(struct page* page);

#endif
