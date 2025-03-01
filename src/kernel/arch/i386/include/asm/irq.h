#ifndef __DENTON_ARCH_I386_ASM_IRQ_H
#define __DENTON_ARCH_I386_ASM_IRQ_H

#include <stdbool.h>
#include <stdint.h>

#include <denton/compiler.h>

enum idt32_gate {
    IDT32_GATE_TASK = 0x5,
    IDT32_GATE_IRQ16 = 0x6,
    IDT32_GATE_TRAP16 = 0x7,
    IDT32_GATE_IRQ32 = 0x8,
    IDT32_GATE_TRAP32 = 0x9,
};

enum irq_type {
    IRQ_INTERRUPT,
    IRQ_SYSCALL,
};

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __packed;

struct idt_entry {
    uint16_t physbase_low;
    uint16_t code_segment;
    uint8_t  reserved;
    enum idt32_gate gate_type : 3;
    uint8_t reserved2;
    uint8_t priviledge : 2;
    uint8_t present : 1;
    uint16_t physbase_high;
} __packed;

void idt_init(void);
void idt_set_entry(struct idt_entry* entry, bool istrap, uint32_t offset, uint8_t priviledge);

#endif

