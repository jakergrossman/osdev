#include "denton/heap.h"
#include "denton/mm/pga.h"
#include "denton/panic.h"
#include <denton/errno.h>
#include <denton/mm/bootmem.h>
#include <denton/math.h>
#include <denton/klog.h>
#include <denton/bits.h>
#include <denton/bits.h>
#include <denton/types.h>
#include <denton/klog.h>
#include <stdint.h>

#include <asm/paging.h>
#include <asm/memlayout.h>

#include <stdlib.h>

extern void* __KERNEL_START;
extern void* __KERNEL_END;

static memory_t memregions[64];
static size_t last_memregion = 0;

int
bootmem_add(uint64_t start, uint64_t end)
{
	const uint64_t __kernel_page_start = V2P(&__KERNEL_START);
	const uint64_t __kernel_page_end = V2P(&__KERNEL_END);

	start = PAGE_ALIGN(start);
	end = PAGE_ALIGN_DOWN(end);

	/* don't allow zero-page to be allocated even if it's valid memory */
	if (start == 0) {
		start = PAGE_SIZE;
		if (start >= end) {
			return 0;
		}
	}

	/* check if this region overlaps with kernel, and exclude
	 * the portions that do
	 */
	if ((start >= __kernel_page_start) && (end < __kernel_page_end)) {
		return 0;
	}

	if (((start <  __kernel_page_end)   && (end >= __kernel_page_end)) ||
		((start <= __kernel_page_start) && (end >  __kernel_page_start)))
	{
		if (start < __kernel_page_start) {
			bootmem_add(start, __kernel_page_start);
		}

		if (end > __kernel_page_end) {
			bootmem_add(__kernel_page_end, end);
		}

		return 0;
	}

	/* check if this region overlaps with vga, and exclude
	 * the portions that do
	 */
	const uint64_t vga_start = 0xB8000;
	const uint64_t vga_end = vga_start + PAGE_SIZE;

	if (((start <  vga_end)   && (end >= vga_end)) ||
		((start <= vga_start) && (end >  vga_start)))
	{
		if (start < vga_start) {
			bootmem_add(start, vga_start);
		}

		if (end > vga_end) {
			bootmem_add(vga_end, end);
		}

		return 0;
	}

	klog_info("registering region 0x%08llX-0x%08llX, %lldKB\n",
			  start, end, (end - start) / __KiX(1)*1024);

	for (size_t i = last_memregion; i < ARRAY_LENGTH(memregions); i++) {
		if (memregions[i].start == 0) {
			memregions[i].start = start;
			memregions[i].end_ex = end;
			last_memregion = i+1;
			return 0;
		}
	}

	klog_warn("ran out of regions, discarded region 0x%08llX-0x%08llX\n", start, end);

	return -ENOMEM;
}

void* __bootmem_alloc_nopanic(size_t len, size_t alignment)
{
	for (memory_t* reg = memregions;
		 reg != (memregions + ARRAY_LENGTH(memregions));
		 reg++)
	{
		physaddr_t first_addr = ALIGN_2(reg->start, alignment);
		physaddr_t end_addr = ALIGN_2_DOWN(reg->end_ex, alignment);

		if ((first_addr < end_addr) && (end_addr - first_addr >= len)) {
			// incrementing this causes it to be unavailable for the page allocator
			reg->start = first_addr + len;
			return p_to_v(first_addr);
		}
	}

	return NULL;
}

void* bootmem_alloc(size_t len, size_t alignment)
{
	void* ret = __bootmem_alloc_nopanic(len, alignment);
	if (!ret) {
		panic("OOM");
	}
	return ret;
}

void bootmem_setup_pga(void)
{
	klog_debug("Initializing page allocator\n");
	int pages = page_alloc_init(&bootmem_ally, memregions, last_memregion);
	klog_info("page allocator has %d free pages\n", pages);
}

#include "allocator.c"

static void* bootmem_ally_alloc(void* ptr, size_t len, pgf_t flags)
{
	return bootmem_alloc(len, pgf_align(flags));
}

static const struct allocator_ops bootmem_ally_ops = {
	.alloc = bootmem_ally_alloc,
	.free = NULL,
};

const struct allocator bootmem_ally = {
	.ptr = NULL,
	.ops = &bootmem_ally_ops,
};
