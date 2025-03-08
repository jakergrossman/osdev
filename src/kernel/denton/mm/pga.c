#include "bitmap_pga.h"

#include "denton/mm/bootmem.h"
#include "denton/types.h"
#include <denton/compiler.h>
#include <denton/atomic.h>
#include <denton/list.h>
#include <denton/mm/pga.h>

#include <asm/paging.h>
#include <asm/memlayout.h>

#include <stdlib.h>

// TODO: compile-time selectable somehow
static const struct pga* __pga = &bitmap_pga;

int  page_alloc_init(const struct allocator* bootally, memory_t* regions, size_t num_regions)
{
	int ret = 0;
	if (__pga->ops->setup) {
		ret = __pga->ops->setup(__pga->ctx, bootally, regions, num_regions);
	}
	return ret;
}


struct page* page_alloc(int order, unsigned int flags)
{
	// WARN_ON(!__pga->ops->alloc)
	return __pga->ops->alloc(__pga->ctx, order, flags);
}


void page_free(struct page* pg, size_t count)
{
	if (__pga->ops->free) {
		__pga->ops->free(__pga->ctx, pg, count);
	}
}


struct page* page_lookup(page_frame_t page_frame)
{
	// WARN_ON(!__pga->ops->lookup)
	struct page* page = __pga->ops->lookup(__pga->ctx, page_frame);
	
	if (page == NULL) {
		return NULL;
	}

	return (page->flags & PAGE_INVALID) ? NULL : page;
}


page_frame_t page_frame_from_physaddr(physaddr_t phys)
{
	return (page_frame_t)(phys >> PAGE_SHIFT);
}


struct page * page_from_phys(physaddr_t phys)
{
	return page_lookup(page_frame_from_physaddr(phys));
}


struct page * page_from_virt(virtaddr_t virt)
{
	return page_lookup(page_frame_from_physaddr(V2P(virt)));
}


physaddr_t page_to_phys(struct page * pg)
{
	return (pg->page_number << PAGE_SHIFT);
}
