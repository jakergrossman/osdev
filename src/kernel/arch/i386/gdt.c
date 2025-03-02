#include "asm/instr.h"
#include "asm/memlayout.h"
#include "denton/types.h"
#include <asm/gdt.h>

extern void __get_gdt(uint32_t* val);
static struct gdt_ptr gdt_ptr;

static inline void
__gdt_load(virtaddr_t addr)
{
    asm volatile (
        "lgdt (%0)" : : "r" (addr)
    );
}

static inline void
__gdt_flush(const struct gdt_ptr* ptr)
{
    cli();
    __gdt_load((virtaddr_t)ptr);

    // asm volatile (
    //     "jmpl $0x08, $1f\n"
    //     "1:\n"
    //     : : : "memory"
    // );
}

void gdt_flush(const struct gdt_entry* gdt, size_t len)
{
    uint32_t val;
    __get_gdt(&val);
    gdt_ptr.base = (uintptr_t)gdt;
    gdt_ptr.limit = sizeof(struct gdt_entry) * len;
    __gdt_flush(&gdt_ptr);
}
