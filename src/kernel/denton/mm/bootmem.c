#include <denton/mm/bootmem.h>
#include <denton/math.h>
#include <denton/klog.h>
#include <denton/types.h>
#include <denton/klog.h>
#include <stdint.h>

#include <asm/paging.h>
#include <asm/memlayout.h>

#include <stdlib.h>

struct bootmem_region {
	physaddr_t start;
	physaddr_t end_ex;
};

extern void* __KERNEL_START;
extern void* __KERNEL_END;

static struct bootmem_region memregions[64];
static pfn_t highest_page;

int
bootmem_add(uint64_t start, uint64_t length)
{
	const pfn_t __kernel_page_start = V2P(&__KERNEL_START);
	const pfn_t __kernel_page_end = V2P(&__KERNEL_END);

	start = PAGE_ALIGN(start);
	uint64_t end = PAGE_ALIGN_DOWN(start + length);

	size_t size = end - start;

	/* don't allow zero-page to be allocated even if it's valid memory */
	if (start == 0) {
		start = PAGE_SIZE;
		if (start >= end) {
			return 0;
		}
	}

	/* keep track of the highest page of physical memory
	 * so we know how much we have for the page allocator
	 */
	pfn_t last_page = __physaddr_to_pfn(__KERNEL_PHYSBASE);
	highest_page = kmax(highest_page, last_page);

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

	klog_info("registering region 0x%08llX-0x%08llX, %dKB\n", start, end, length / __KiX(1));

	for (size_t i = 0; i < ARRAY_LENGTH(memregions); i++) {
		if (memregions[i].start == 0) {
			memregions[i].start = start;
			memregions[i].end_ex = end;
			return 0;
		}
	}

	klog_warn("ran out of regions, discardad region 0x%08llX-0x%08llX\n", start, end, length / __KiX(1));

	// TODO return ENOMEM
	return -1;
}

void* bootmem_alloc(size_t len, size_t alignment)
{
	for (struct bootmem_region* reg = memregions;
		 reg != (memregions + ARRAY_LENGTH(memregions));
		 reg++)
	{
		physaddr_t first_addr = ALIGN_2(reg->start, alignment);
		physaddr_t end_addr = ALIGN_2_DOWN(reg->end_ex, alignment);

		if ((first_addr < end_addr) && (end_addr - first_addr >= len)) {
			reg->start = first_addr + len;
			return p_to_v(first_addr);
		}
	}

	return NULL;
}
