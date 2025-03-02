#include "include/asm/irq.h"
#include "asm/instr.h"
#include "asm/drivers/pic8259.h"
#include "denton/atomic.h"
#include "denton/heap.h"
#include "denton/klog.h"
#include "denton/kstring.h"
#include "denton/math/minmax.h"
#include <denton/list.h>
#include <asm/irq.h>
#include <asm/atomic.h>
#include <stdbool.h>
#include <stdint.h>

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

extern void (*irq_hands[256])(void);
void idt_init(void)
{
    idt_ptr.limit = sizeof(idt_entries) - 1;
    idt_ptr.base = (uintptr_t)&idt_entries;

    for (int i = 0; i < 256; i++) {
        list_init(&idt_ids[i].list);
        idt_set_entry(&idt_entries[i], false, 1 << 3, (uint32_t)(irq_hands[i]), 0);
    }

    idt_flush((uintptr_t)&idt_ptr);
}

#define STS_TG32 0xF
#define STS_IG32 0xF

void idt_set_entry(struct idt_entry* entry, bool istrap, uint32_t offset,
                   uint16_t sel, uint8_t priviledge)
{
    struct idt_entry updated = {
        .physbase_low = offset & 0xFFFF,
        .physbase_high = (offset >> 16) & 0xFFFF,
        .code_segment = sel,
        .reserved = 0,
        .reserved2 = 0,
        .gate_type = istrap ? STS_TG32 : STS_IG32,
        .priviledge = priviledge,
        .present = true,
    };
    *entry = updated;
}

int irq_register_handler(
	const char* name,
	int irqno,
	irqfn_t irqfn,
	enum irq_type type,
	void* privdata
)
{
	struct irq_handler* handler = kzalloc(sizeof(*handler), 0);
	if (!handler) {
		// TODO: return ENOMEM
		return -1;
	}
	irq_handler_init(handler);

	// FIXME:
	handler->name = (char*)name;
	if (!handler->name) {
		goto free_name;
	}

	handler->type = type;
	handler->irqfn = irqfn;

	int err = x86_register_irq_handler(irqno + PIC8259_IRQ0, handler);
	if (err) {
		goto free_name;
	}

	return 0;

free_name:
	kfree(handler, sizeof(*handler));
	return -1;
}

int x86_register_irq_handler(uint8_t irqno, struct irq_handler* hand)
{
	int err = 0;
	bool enable = false;

	/* FIXME: we don't suuport SMP, so just turn off IRQs instead fo proper locking */
	irq_flags_t flags = irq_save();
	irq_disable();

	struct idt_id* ident = idt_ids + irqno;
	if (!list_empty(&ident->list)) {
		if (ident->type != hand->type) {
			goto restore_flags;
		}
	} else {
		enable = true;
		ident->type = hand->type;
		ident->flags = hand->flags;
	}

	klog_info("interrupt %d, name: %s\n", irqno, hand->name);

	list_add_tail(&ident->list, &hand->listentry);

	if (enable && (irqno >= PIC8259_IRQ0) && (irqno <= (PIC8259_IRQ0 + 16))) {
		pic8259_enable_irq(irqno - PIC8259_IRQ0);
	}

restore_flags:
	irq_restore(flags);
	return err;
}

void irq_global_handler(struct irq_frame* iframe)
{
	printf("I MADE IT\n");
	struct idt_id* ident = idt_ids + iframe->intno;
	int pic8259_irq = -1;

	atomic_inc(&ident->count);

	if (kinrange(iframe->intno, PIC8259_IRQ0, PIC8259_IRQ0+16)) {
		pic8259_irq = iframe->intno - PIC8259_IRQ0;

		pic8259_disable_irq(pic8259_irq);
		pic8259_send_eoi(pic8259_irq);
	}

	struct irq_handler* hand;
	list_for_each_entry(hand, &ident->list, listentry) {
		hand->irqfn(iframe, hand->privdata);
	}

	if (pic8259_irq >= 0) {
		pic8259_enable_irq(pic8259_irq);
	}

	cli();
}
