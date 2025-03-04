#ifndef __DENTON_ARCH_I386_ASM_IRQ_H
#define __DENTON_ARCH_I386_ASM_IRQ_H

#include "asm/instr.h"
#include <denton/atomic.h>
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

typedef int irq_flags_t;
#define irq_disable() cli()
#define irq_enable() sti()

static inline __must_check uint32_t
irq_save(void)
{
    return eflags_read();
}

#define irq_save() eflags_read()
#define irq_restore(flags) eflags_write(flags)

#endif

