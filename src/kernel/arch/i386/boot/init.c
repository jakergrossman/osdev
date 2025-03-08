#include "denton/heap.h"
#include <denton/klog.h>
#include <denton/kmain.h>
#include <denton/kparam.h>
#include <denton/tty.h>
#include <denton/panic.h>
#include <denton/multiboot.h>
#include <denton/mm/bootmem.h>

#include <asm/cpuid.h>
#include <asm/cpu.h>
#include <asm/memlayout.h>
#include <asm/paging.h>
#include <asm/drivers/pic8259.h>
#include <asm/instr.h>
#include <asm/irq.h>
#include <asm/idt.h>

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


	bootmem_add(start, start+length);
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
		char* cmdp = (char*)((uintptr_t)__kernel_cmdline - __KERNEL_VIRTBASE);
		strcpy(cmdp, info->cmdline);
	}

	struct multiboot_memmap* mmap = p_to_v(info->mmap_addr);

	while (V2P(mmap) < info->mmap_addr + info->mmap_length) {
		if (mmap->type == MB_MEMMAP_AVAIL) {
			add_multiboot_memory_region(mmap->base_addr, mmap->length);
		}
		mmap = (struct multiboot_memmap*)((uint32_t)mmap + mmap->size + sizeof(uint32_t));
	}
}

void cmain(uint32_t magic, struct multiboot_info* mb_info)
{
	cpuid_init();

	cpu_early_init();

	klog_init();

	terminal_initialize(EARLY_BOOT_VGA);

	klog_info("==< DENTON BOOTING >==\n");
	klog_info("kernel physical location: 0x%8p-0x%8p\n", 
			(void*)V2P(&__KERNEL_START), (void*)V2P(&__KERNEL_END));
	klog_info("kernel virtual  location: 0x%8p-0x%8p\n", &__KERNEL_START, &__KERNEL_END);

	if (magic == MULTIBOOT_BOOTLOADER_MAGIC) {
		handle_multiboot_info(mb_info);
	} else {
		panic("bad magic");
	}

	idt_init();

	/* set up our permanent kernel space mappng */
	paging_setup_kernelspace();

	/* handoff from bootmem to physical frame allocator */
	bootmem_setup_pga();

	/* now that we have dynamic memory, let's handle the command line */
	kernel_cmdline_init(__kernel_cmdline);

	/* setup interrupt controller */
	pic8259_init();
	pic8259_timer_init();

	kmain();
}
