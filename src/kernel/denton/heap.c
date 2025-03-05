#include <stddef.h>
#include <stdint.h>

#include <denton/bits.h>
#include <denton/mm/pga.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "heap.h"

// TODO: real allocator
void* kmalloc(size_t size, uint32_t flags)
{
	// really dumb, for now
	size_t required_pages = (size + PAGE_SIZE-1) / PAGE_SIZE;
	struct page* start_page = page_alloc(required_pages, flags);
	return start_page->virt;
}

void* kzalloc(size_t size, uint32_t flags)
{
	void* mem = kmalloc(size, flags);
	if(!mem) {
		return NULL;
	}
	memset(mem, 0, size);
	return mem;
}

void kfree(void* mem, size_t size)
{
	// really dumb, for now
	size_t required_pages = (size + PAGE_SIZE-1) / PAGE_SIZE;
	page_free(page_from_virt(mem), required_pages);
}

void  kheap_init(void)
{
}
