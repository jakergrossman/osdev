#ifndef __DENTON_ARCH_I386_ASM_IRQ_H
#define __DENTON_ARCH_I386_ASM_IRQ_H


#include <denton/atomic.h>
#include <denton/list.h>
#include <denton/types.h>
#include <denton/compiler.h>

#include <stdbool.h>
#include <stdint.h>

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
void idt_set_entry(struct idt_entry* entry, bool istrap, uint32_t offset,
                   uint16_t sel, uint8_t priviledge);


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

typedef void (*irqfn_t)(struct irq_frame * frame, void * privdata);

struct irq_handler {
    char* name;
	irqfn_t irqfn;
    void* privdata;
    enum irq_type type;
	struct list_head listentry;
	unsigned int flags;
};

static __always_inline void
irq_handler_init(struct irq_handler* hand)
{
	*hand = (struct irq_handler){ .listentry = LIST_HEAD_INIT(hand->listentry) };
}

struct idt_id {
    atomic_t count;
    enum irq_type type;
    uint32_t flags;
    struct list_head list;
};


void idt_init(void);
void idt_flush(physaddr_t phys);

int x86_register_irq_handler(uint8_t irqno, struct irq_handler* hand);

int irq_register_handler(
	const char* name,
	int irqno,
	irqfn_t irqfn,
	enum irq_type type,
	void* privdata
);

typedef int irq_flags_t;
#define irq_disable() cli()
#define irq_save() eflags_read()
#define irq_restore(flags) eflags_write(flags)

#endif

