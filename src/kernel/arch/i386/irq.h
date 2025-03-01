#ifndef _I386_IDT_H
#define _I386_IDT_H

#include "denton/types.h"
#include <denton/compiler.h>

#include <stdint.h>

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
void idt_flush(physaddr_t phys);

#endif
