#include "include/asm/irq.h"
#include "asm/gdt.h"
#include "asm/idt.h"
#include "asm/instr.h"
#include "asm/drivers/pic8259.h"
#include "asm/memlayout.h"
#include "denton/atomic.h"
#include "denton/heap.h"
#include "denton/klog.h"
#include "denton/kstring.h"
#include "denton/math/minmax.h"
#include "denton/tty.h"
#include "denton/types.h"
#include <denton/list.h>
#include <asm/irq.h>
#include <asm/atomic.h>
#include <stdbool.h>
#include <stdint.h>


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

    struct idt_id* ident = idt_get_id(irqno);
    if (!list_empty(&ident->list)) {
        // if (ident->type != hand->type) {
        //     goto restore_flags;
        // }
    } else {
        enable = true;
        // ident->type = hand->type;
        ident->flags = hand->flags;
    }

    klog_info("interrupt %d, name: %s\n", irqno, hand->name);

    list_add_tail(&ident->list, &hand->listentry);

    if (enable && (irqno >= PIC8259_IRQ0) && (irqno <= (PIC8259_IRQ0 + 16))) {
        pic8259_enable_irq(irqno - PIC8259_IRQ0);
    }

// restore_flags:
    irq_restore(flags);
    return err;
}

void irq_global_handler(struct irq_frame* iframe)
{
    printf("edi:  %08X\n", iframe->edi);
    printf("esi:  %08X\n", iframe->esi);
    printf("ebp:  %08X\n", iframe->ebp);
    printf("oesp: %08X\n", iframe->oesp);
    printf("ebx:  %08X\n", iframe->ebx);
    printf("edx:  %08X\n", iframe->edx);
    printf("ecx:  %08X\n", iframe->edi);
    printf("eax:  %08X\n", iframe->eax);
    printf("gs:       %04X\n", iframe->gs);
    printf("fs:       %04X\n", iframe->fs);
    printf("es:       %04X\n", iframe->es);
    printf("ds:       %04X\n", iframe->ds);
    printf("irq:  %08X\n", iframe->intno);
    printf("err:  %08X\n", iframe->err);
    printf("eip:  %08X\n", iframe->eip);
    printf("cs:       %04X\n", iframe->cs);
    printf("eflg: %08X\n", iframe->eflags);
    printf("esp:  %08X\n", iframe->esp);
    printf("ss:       %04X\n", iframe->ss);
    terminal_flush();
    hlt();

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

    cli();
}
