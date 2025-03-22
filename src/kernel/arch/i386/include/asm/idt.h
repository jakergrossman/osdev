#ifndef __DENTON_ARCH_1386_ASM_IDT_H
#define __DENTON_ARCH_1386_ASM_IDT_H

/** Interrupt Descriptor Table (IDT) Handling Routines */

#include "denton/list.h"
#include <denton/compiler.h>

#include <asm/irq.h>

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

enum idt32_gate {
	IDT32_GATE_TASK = 0x5,
	IDT32_GATE_IRQ16 = 0x6,
	IDT32_GATE_TRAP16 = 0x7,
	IDT32_GATE_IRQ32 = 0xE,
	IDT32_GATE_TRAP32 = 0xF,
};

struct idt_ptr {
	uint16_t limit;
	uint32_t base;
} __packed;

struct idt_entry {
	uint16_t physbase_low;
	uint16_t seg_select;
	uint8_t  reserved;
	enum idt32_gate gate_type : 4;
	uint8_t reserved2 : 1;
	uint8_t priviledge : 2;
	uint8_t present : 1;
	uint16_t physbase_high;
} __packed;

struct idt_id {
	_Atomic(unsigned long) count;
	uint32_t flags;
	struct list_head list;
};

void idt_init(void);
void idt_flush(void);
void idt_set_entry(size_t irqno, bool istrap, uint16_t sel, uint8_t priviledge);
struct idt_id * idt_get_id(size_t idtno);
int idt_register_irq_handler(uint8_t irqno, struct irq_handler* hand);

#endif
