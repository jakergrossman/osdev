#ifndef __DENTON_ARCH_I386_ASM_IRQ_H
#define __DENTON_ARCH_I386_ASM_IRQ_H

#include "asm/instr.h"
#include "denton/bits/bits.h"
#include "sys/compiler.h"
#include <denton/list.h>
#include <denton/types.h>
#include <denton/compiler.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <asm/irq_handler.h>

enum irq_type {
	IRQ_INTERRUPT,
	IRQ_SYSCALL,
};


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

#define IRQ_HANDLER_INIT(hd, nm, hand, priv, typ, fl) \
	{ \
		.name = nm, \
		.irqfn = hand, \
		.privdata = priv, \
		.type = typ, \
		.flags = fl, \
		.listentry = LIST_HEAD_INIT((hd).listentry), \
	}



int irq_register_handler(
	const char* name,
	int irqno,
	irqfn_t irqfn,
	enum irq_type type,
	void* privdata
);

typedef unsigned long irq_flags_t;
#define irq_disable() cli()
#define irq_enable() sti()

static __always_inline __must_check uint32_t
irq_save(void)
{
	uint32_t flags = eflags_read();
	irq_disable();
	return flags;
}

static __always_inline int arch_irqs_disabled_flags(uint32_t flags)
{
	return !(flags & BIT(9)); // FIXME: magic number
}

static __always_inline void
irq_restore(uint32_t flags)
{
	if (!arch_irqs_disabled_flags(flags)) {
		irq_enable();
	}
}

#endif

