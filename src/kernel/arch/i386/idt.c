#include <denton/compiler.h>
#include <denton/atomic.h>
#include <denton/list.h>
#include <denton/types.h>

#include <asm/memlayout.h>
#include <asm/gdt.h>
#include <asm/idt.h>
#include <asm/irq.h>

#include <sys/cdefs.h>
#include <stdlib.h>

/* kernel-defined book keeping for IDT entries */
static struct idt_id idt_ids[256];

/* x86 IDT entry table */
static struct idt_entry idt_entries[256] = { 0 };

/* directly load the IDT pointer and size into the IDT register */
static __always_inline void
__idt_load(struct idt_ptr * addr)
{
    asm volatile (
        "lidt (%0)" : : "r" (addr)
    );
}

/* construct the IDT pointer structure, then flush the IDT register with it */
static __always_inline void 
__idt_flush(const struct idt_entry * base, size_t len)
{
    struct idt_ptr idt_ptr = {
        .base = (uintptr_t)base,
        .limit = sizeof(*base)*len - 1,
    };
    __idt_load((virtaddr_t)&idt_ptr);
}

/* load the IDT table given a base address and entry count */
void idt_flush(void)
{
    __idt_flush(idt_entries, ARRAY_LENGTH(idt_entries));
}

/* initialize the IDT table, and set up some basic exception handlers */
void idt_init(void)
{
    for (int i = 0; i < 256; i++) {
        list_init(&idt_ids[i].list);
        idt_set_entry(i, false, GDT_KERNEL_CODE, GDT_DPL_KERNEL);
    }

    idt_flush();
}

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

    uint32_t virtbase = (uint32_t)irq_handlers[idtno];
    struct idt_entry updated = {
        .physbase_low = virtbase & 0xFFFF,
        .physbase_high = (virtbase >> 16) & 0xFFFF,
        .seg_select = sel << 3,
        .reserved = 0,
        .reserved2 = 0,
        .gate_type = istrap ? IDT32_GATE_TRAP32 : IDT32_GATE_IRQ32,
        .priviledge = priviledge,
        .present = true,
    };
    idt_entries[idtno] = updated;
}

