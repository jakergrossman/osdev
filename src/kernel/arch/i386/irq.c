#include "include/asm/irq.h"
#include "irq.h"
#include <denton/list.h>
#include <asm/irq.h>
#include <asm/atomic.h>
#include <stdbool.h>
#include <stdint.h>

struct idt_id {
    atomic32_t count;
    enum irq_type type;
    uint32_t flags;
    struct list_head list;
};

static struct idt_id idt_ids[256];
static struct idt_entry idt_entries[256] = { 0 };
static struct idt_ptr idt_ptr;

void idt_flush(physaddr_t phys)
{
    asm volatile (
        "movl 4(%esp), %eax\n\t"
        "lidt (%eax)\n\t"
    );
}

void idt_init(void)
{
    idt_ptr.limit = sizeof(idt_entries) - 1;
    idt_ptr.base = (uintptr_t)&idt_entries;

    for (int i = 0; i < 256; i++) {
        list_init(&idt_ids[i].list);
        // idt_set_entry(&idt_ids[i], false, __KERNEL_CS, uint8_t priviledge)
    }

    idt_flush((uintptr_t)&idt_ptr);
}

#define STS_TG32 0xF
#define STS_IG32 0xF

void idt_set_entry(struct idt_entry* entry, bool istrap, uint32_t offset,
                   uint16_t sel, uint8_t priviledge)
{
    struct idt_entry updated = {
        .base_low = offset & 0xFFFF,
        .base_high = (offset >> 16) & 0xFFFF,
        .code_segment = sel,
        .reserved = 0,
        .reserved2 = 0,
        .type = istrap ? STS_TG32 : STS_IG32,
        .priviledge = priviledge,
        .present = true,
    };
    *entry = updated;
}
