#ifndef __DENTON_ARCH_I386_ASM_IRQ_H
#define __DENTON_ARCH_I386_ASM_IRQ_H


#include <denton/atomic.h>
#include <denton/list.h>
#include <denton/types.h>
#include <denton/compiler.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

extern void (*irq_hands[256])(void);

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

