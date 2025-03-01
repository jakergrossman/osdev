#ifndef _I386_IDT_H
#define _I386_IDT_H

#include <denton/compiler.h>

#include <stdint.h>
enum idt32_gate {
    IDT32_GATE_TASK = 0x5,
    IDT32_GATE_IRQ16 = 0x6,
    IDT32_GATE_TRAP16 = 0x7,
    IDT32_GATE_IRQ32 = 0x8,
    IDT32_GATE_TRAP32 = 0x9,
};


struct idt_entry {
    uint16_t physbase_low;
    uint16_t code_segment;
    uint8_t  reserved;
    enum idt32_gate gate_type : 4;
    uint8_t priviledge : 2;
    uint8_t present : 1;
    uint16_t physbase_high;
} __packed;

struct irq_frame {
    uint32_t edi, esi, ebp, oesp, ebx, edx, ecx, eax;
    uint16_t gs, pad1, fs, pad2, es, pad3, ds, pad4;

    uint32_t intno;
    uint32_t err;

    uint32_t eip;
    uint16_t cs, pad5;
    uint32_t eflags;

    uint32_t esp;
    uint16_t ss, pad6;
} __packed;

enum irq_type {
    IRQ_INTERRUPT,
    IRQ_SYSCALL,
};

struct irq_handler {
    const char* name;
    void (*fn)(struct irq_frame*, void*);
    void* privdata;
    enum irq_type type;
};

void idt_init(void);
void idt_flush(void);

#endif
