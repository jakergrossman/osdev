#include "asm/instr.h"
#include "asm/memlayout.h"
#include "denton/types.h"
#include <asm/gdt.h>

static __always_inline void
__gdt_load(virtaddr_t addr)
{
    asm volatile (
        "lgdt (%0)" : : "r" (addr)
    );
}

static __always_inline void
__gdt_flush(const struct gdt_ptr* ptr)
{
    cli();

    __gdt_load((virtaddr_t)ptr);

    /* reload CS and IP */
    asm volatile (
        "ljmp $0x08, $boing\n"
        "boing:\n"
        : : : "memory"
    );

    /* set the segment registers to use the new GDT */
    asm volatile (
        "movw %w0, %%ss\n"
        "movw %w0, %%ds\n"
        "movw %w0, %%es\n"
        "movw %w0, %%fs\n"
        : : "r" (GDT_KERNEL_DATA<<3)
        : "memory"
    );
}

void gdt_flush(const struct gdt_entry* gdtbase, size_t len)
{
    struct gdt_ptr gdt_ptr = {
        .base = (uintptr_t)gdtbase,
        .limit = sizeof(*gdtbase)*len - 1,
    };
    __gdt_flush((virtaddr_t)&gdt_ptr);
}
