#include "asm/cpu.h"
#include "denton/sched.h"
#include <denton/heap.h>
#include <denton/klog.h>
#include <denton/kstring.h>
#include <denton/math.h>
#include <denton/atomic.h>
#include <denton/tty.h>
#include <denton/types.h>
#include <denton/errno.h>

#include <asm/instr.h>
#include <asm/gdt.h>
#include <asm/idt.h>
#include <asm/irq.h>
#include <asm/irq_handler.h>

#include <asm/drivers/pic8259.h>

#include <stdbool.h>
#include <stdint.h>
#include <limits.h>

#include <denton/bits/bitmap.h>


int irq_register_handler(
	const char* name,
	int irqno,
	irqfn_t irqfn,
	enum irq_type type,
	void* privdata
)
{
	int ret = 0;

	struct irq_handler* handler = kzalloc(sizeof(*handler), 0);
	if (!handler) {
		return -ENOMEM;
	}
	irq_handler_init(handler);

	// FIXME:
	handler->name = (char*)name;
	if (!handler->name) {
		goto free_handler;
	}

	handler->type = type;
	handler->irqfn = irqfn;

	klog_info("irq %d, '%s'\n", irqno, handler->name);
	ret = idt_register_irq_handler(irqno + PIC8259_IRQ0, handler);
	if (ret) {
		goto free_name;
	}

	return 0;

free_name:
	kfree((char*)name, strlen(name));
free_handler:
	kfree(handler, sizeof(*handler));

	return ret;
}


void irq_global_handler(struct irq_frame* iframe)
{
	struct idt_id* ident = idt_get_id(iframe->intno);
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

	if (cpu_get_local()->allow_preempt && cpu_get_local()->reschedule) {
		sched_yield();
		cpu_get_local()->reschedule = false;
	}
}
