#ifndef __IRQ_HANDLER_H
#define __IRQ_HANDLER_H

#include <stdint.h>

/** unified irq data about processor state on IRQ entry */
struct irq_frame {
    /* general purpose registers */
    uint32_t edi, esi, ebp, oesp, ebx, edx, ecx, eax;

    /* data segment registers */
    uint16_t gs, pad1;
    uint16_t fs, pad2;
    uint16_t es, pad3;
    uint16_t ds, pad4;

    /* interrupt number */
    uint32_t intno;
    uint32_t err;

    /* instruction pointer */
    uint32_t eip;
    /* code segment register */
    uint16_t cs, pad5;
    uint32_t eflags;

    /* stack pointer */
    uint32_t esp;
    /* stack segment register */
    uint16_t ss, pad6;
} __packed;

extern void (*irq_handlers[256])(void);

void irq_global_handler(struct irq_frame* iframe);

#endif
