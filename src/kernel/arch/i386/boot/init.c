#include <denton/klog.h>
#include <denton/kmain.h>
#include <denton/tty.h>
#include <denton/panic.h>
#include <denton/multiboot.h>
#include <denton/mm/bootmem.h>

#include <asm/cpuid.h>
#include <asm/memlayout.h>
#include <asm/paging.h>

#include <stdio.h>
#include <string.h>
#include <limits.h>

extern void* __KERNEL_START;
extern void* __KERNEL_END;
char __kernel_cmdline[PAGE_SIZE];

static void
add_multiboot_memory_region(uint64_t start, size_t length)
{
	if (start > INT_MAX) {
		return;
	}

	if (start + length > INT_MAX) {
		length = INT_MAX - start;
	}

	bootmem_add(start, length);
}

static void
handle_multiboot_info(struct multiboot_info* info)
{
	/* we haven't started paging, and the first megabyte
	 * of the address space is identity mapped. copy the
	 * kernel command line out now so it doesn't get clobbered
	 */
	if ((info->flags & MBF_INFO_CMDLINE) && (*info->cmdline)) {
		/* don't overflow */
		info->cmdline[sizeof(__kernel_cmdline)-1] = 0;
		strcpy(__kernel_cmdline, info->cmdline);
		klog(KLOG_INFO, "cmdline: %s\n", __kernel_cmdline);
	}

	struct multiboot_memmap* mmap = p_to_v(info->mmap_addr);

	while (V2P(mmap) < info->mmap_addr + info->mmap_length) {
		if (mmap->type == 1) {
			uint32_t start = mmap->base_addr;
			uint32_t end = start + mmap->length;
			klog_info("start: %x, end: %x\n", start, end);
			add_multiboot_memory_region(start, end);
		}
		mmap = (struct multiboot_memmap*)((uint32_t)mmap + mmap->size + sizeof(uint32_t));
	}
}

void cmain(uint32_t magic, struct multiboot_info* mb_info)
{
	cpuid_init();

	terminal_initialize(EARLY_BOOT_VGA);

	klog_info("denton booting...\n");
	klog_info("kernel physical location: 0x%X-0x%X\n", V2P(&__KERNEL_START), V2P(&__KERNEL_END));

	if (magic == MULTIBOOT_BOOTLOADER_MAGIC) {
		handle_multiboot_info(mb_info);
	} else {
		panic("bad magic");
	}

	klog_trace("setting up kernelspace paging...\n");
	paging_setup_kernelspace();
	// FIXME:
	terminal_update_base(INIT_VGA);

	klog_trace("setting bootmem physical allocator...\n");
	klog_trace("setting kmalloc...\n");
	klog_trace("setting pic8259...\n");
	klog_trace("setting pic8259 timer...\n");
	klog_trace("reading RTC...\n");

	kmain();
}
