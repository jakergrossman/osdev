#include "denton/compiler.h"
#include <denton/atomic.h>
#include <denton/list.h>
#include <denton/types.h>

#include <asm/memlayout.h>
#include <asm/gdt.h>
#include <asm/idt.h>
#include <asm/irq.h>

#include <sys/cdefs.h>
#include <stdlib.h>

static struct idt_id idt_ids[256];
static struct idt_entry idt_entries[256] = { 0 };
static struct idt_ptr idt_ptr;

static __always_inline void
__idt_load(virtaddr_t addr)
{
    asm volatile (
        "lidt (%0)" : : "r" (addr)
    );
}

static __always_inline void 
__idt_flush(const struct idt_entry* base, size_t len)
{
    idt_ptr.base = (uintptr_t)base;
    idt_ptr.limit = sizeof(*base)*len - 1;
    __idt_load((virtaddr_t)&idt_ptr);
}

void idt_flush(void)
{
    __idt_flush(idt_entries, ARRAY_LENGTH(idt_entries));
}

void idt_init(void)
{
    for (int i = 0; i < 256; i++) {
        list_init(&idt_ids[i].list);
        idt_set_entry(i, false, GDT_KERNEL_CODE, GDT_DPL_KERNEL);
    }

    idt_flush();
}

#define STS_TG32 0xF
#define STS_IG32 0xE

struct idt_id*
idt_get_id(size_t idtno)
{
    if (idtno < ARRAY_LENGTH(idt_ids)) {
        return &idt_ids[idtno];
    }
    return NULL;
}

void idt_set_entry(
    size_t idtno,
    bool istrap,
    uint16_t sel,
    uint8_t priviledge)
{
    if (idtno >= ARRAY_LENGTH(idt_entries)) {
        return;
    }

    uint32_t virtbase = (uint32_t)irq_hands[idtno];
    struct idt_entry updated = {
        .physbase_low = virtbase & 0xFFFF,
        .physbase_high = (virtbase >> 16) & 0xFFFF,
        .seg_select = sel << 3,
        .reserved = 0,
        .reserved2 = 0,
        .gate_type = istrap ? STS_TG32 : STS_IG32,
        .priviledge = priviledge,
        .present = true,
    };
    idt_entries[idtno] = updated;
}

