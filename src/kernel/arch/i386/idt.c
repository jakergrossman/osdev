#include "asm/instr.h"
#include "denton/klog.h"
#include "denton/tty.h"
#include <denton/compiler.h>
#include <denton/atomic.h>
#include <denton/list.h>
#include <denton/types.h>

#include <asm/memlayout.h>
#include <asm/gdt.h>
#include <asm/idt.h>
#include <asm/irq.h>
#include <asm/drivers/pic8259.h>

#include <stdio.h>
#include <sys/cdefs.h>
#include <stdlib.h>

/* kernel-defined book keeping for IDT entries */
static struct idt_id idt_ids[256];

/* x86 IDT entry table */
static struct idt_entry idt_entries[256] = { 0 };

static const char *cpu_exception_name[32] = {
    [0] = "Divide by zero",
    [1] = "Debug",
    [2] = "NMI",
    [3] = "Breakpoint",
    [4] = "Overflow",
    [5] = "Bound Range Exceeded",
    [6] = "Invalid OP",
    [7] = "Device Not Available",
    [8] = "Double Fault",
    [10] = "Invalid TSS",
    [11] = "Segment Not Present",
    [12] = "Stack-Segment Fault",
    [13] = "General Protection Fault",
    [14] = "Page Fault",
    [16] = "Floating-Point Exception",
    [17] = "Alignment Check",
    [18] = "Machine Check",
    [19] = "SIMD Floating-Point Exception",
    [20] = "Virtualization Exception",
    [30] = "Security Exception",
};

static void
unhandled_cpu_exception(struct irq_frame* frame, void* privdata)
{
    printf( "[ 0.000000 ] ===== Exception: %s(%d)!!! =====\n",
            cpu_exception_name[frame->intno], frame->intno);
    printf( "[ 0.000000 ] ERR: 0x%08X   EIP: V@%p\n", frame->err, (void *)frame->eip);
    printf( "[ 0.000000 ] EAX: 0x%08X   EBX: 0x%08X\n", frame->eax, frame->ebx);
    printf( "[ 0.000000 ] ECX: 0x%08X   EDX: 0x%08X\n", frame->ecx, frame->edx);
    printf( "[ 0.000000 ] ESI: 0x%08X   EDI: 0x%08X\n", frame->esi, frame->edi);
    printf( "[ 0.000000 ] ESP: 0x%08X   EBP: 0x%08X\n", frame->esp, frame->ebp);
    printf( "[ 0.000000 ] CS:  0x%04X       SS:  0x%04X\n", frame->cs, frame->ss);
    printf( "[ 0.000000 ] DS:  0x%04X       ES:  0x%04X\n", frame->ds, frame->es);
    printf( "[ 0.000000 ] FS:  0x%04X       GS:  0x%04X\n", frame->fs, frame->gs);

    terminal_flush();

    cli(); /* should already be */
    while(1) {
        hlt();
    }
}

static void
div_by_zero_handler(struct irq_frame* frame, void* priv)
{
    // TODO: Sometimes we could just kill the process, guess I'll find out later
    unhandled_cpu_exception(frame, priv);
}

static struct irq_handler cpu_exceptions[] = {
    [0] = IRQ_HANDLER_INIT(cpu_exceptions[0], "Divide By Zero", div_by_zero_handler, NULL, IRQ_INTERRUPT, 0),
    [1] = IRQ_HANDLER_INIT(cpu_exceptions[0], "Debug", unhandled_cpu_exception, NULL, IRQ_INTERRUPT, 0),
    [2] = IRQ_HANDLER_INIT(cpu_exceptions[2], "NMI", unhandled_cpu_exception, NULL, IRQ_INTERRUPT, 0),
    [3] = IRQ_HANDLER_INIT(cpu_exceptions[3], "Breakpoint", unhandled_cpu_exception, NULL, IRQ_INTERRUPT, 0),
    [4] = IRQ_HANDLER_INIT(cpu_exceptions[4], "Overflow", unhandled_cpu_exception, NULL, IRQ_INTERRUPT, 0),
    [5] = IRQ_HANDLER_INIT(cpu_exceptions[5], "Bound Range Exceeded", unhandled_cpu_exception, NULL, IRQ_INTERRUPT, 0),
    [6] = IRQ_HANDLER_INIT(cpu_exceptions[6], "Invalid OP", unhandled_cpu_exception, NULL, IRQ_INTERRUPT, 0),
    [7] = IRQ_HANDLER_INIT(cpu_exceptions[7], "Device Not Available", unhandled_cpu_exception, NULL, IRQ_INTERRUPT, 0),
    [8] = IRQ_HANDLER_INIT(cpu_exceptions[8], "Double Fault", unhandled_cpu_exception, NULL, IRQ_INTERRUPT, 0),
    [10] = IRQ_HANDLER_INIT(cpu_exceptions[10], "Invalid TSS", unhandled_cpu_exception, NULL, IRQ_INTERRUPT, 0),
    [11] = IRQ_HANDLER_INIT(cpu_exceptions[11], "Segment Not Present", unhandled_cpu_exception, NULL, IRQ_INTERRUPT, 0),
    [12] = IRQ_HANDLER_INIT(cpu_exceptions[12], "Stack-Segment Fault", unhandled_cpu_exception, NULL, IRQ_INTERRUPT, 0),
    [13] = IRQ_HANDLER_INIT(cpu_exceptions[13], "General Protection Fault", unhandled_cpu_exception, NULL, IRQ_INTERRUPT, 0),
    [16] = IRQ_HANDLER_INIT(cpu_exceptions[16], "Floating-Point Exception", unhandled_cpu_exception, NULL, IRQ_INTERRUPT, 0),
    [17] = IRQ_HANDLER_INIT(cpu_exceptions[17], "Alignment Check", unhandled_cpu_exception, NULL, IRQ_INTERRUPT, 0),
    [18] = IRQ_HANDLER_INIT(cpu_exceptions[18], "Machine Check", unhandled_cpu_exception, NULL, IRQ_INTERRUPT, 0),
    [19] = IRQ_HANDLER_INIT(cpu_exceptions[19], "SIMD Floating-Point Exception", unhandled_cpu_exception, NULL, IRQ_INTERRUPT, 0),
    [20] = IRQ_HANDLER_INIT(cpu_exceptions[20], "Virtualization Exception", unhandled_cpu_exception, NULL, IRQ_INTERRUPT, 0),
    [30] = IRQ_HANDLER_INIT(cpu_exceptions[30], "Security Exception", unhandled_cpu_exception, NULL, IRQ_INTERRUPT, 0),
};

/* directly load the IDT pointer and size into the IDT register */
static __always_inline void
__idt_load(struct idt_ptr * addr)
{
    asm volatile (
        "lidt (%0)" : : "r" (addr)
    );
}

/* construct the IDT pointer structure, then flush the IDT register with it */
static __always_inline void 
__idt_flush(const struct idt_entry * base, size_t len)
{
    struct idt_ptr idt_ptr = {
        .base = (uintptr_t)base,
        .limit = sizeof(*base)*len - 1,
    };
    __idt_load((virtaddr_t)&idt_ptr);
}

/* load the IDT table given a base address and entry count */
void idt_flush(void)
{
    __idt_flush(idt_entries, ARRAY_LENGTH(idt_entries));
}

/* initialize the IDT table, and set up some basic exception handlers */
void idt_init(void)
{
    for (int i = 0; i < 256; i++) {
        list_init(&idt_ids[i].list);
        idt_set_entry(i, false, GDT_KERNEL_CODE, GDT_DPL_KERNEL);
    }

    for (size_t i = 0; i < ARRAY_LENGTH(cpu_exceptions); i++) {
        if (cpu_exceptions[i].irqfn) {
            idt_register_irq_handler(i, &cpu_exceptions[i]);
        }
    }

    idt_flush();
}

struct idt_id*
idt_get_id(size_t idtno)
{
    if (idtno < ARRAY_LENGTH(idt_ids)) {
        return &idt_ids[idtno];
    }
    return NULL;
}

void idt_set_entry(
    size_t idtno,
    bool istrap,
    uint16_t sel,
    uint8_t priviledge)
{
    if (idtno >= ARRAY_LENGTH(idt_entries)) {
        return;
    }

    uint32_t virtbase = (uint32_t)irq_handlers[idtno];
    struct idt_entry updated = {
        .physbase_low = virtbase & 0xFFFF,
        .physbase_high = (virtbase >> 16) & 0xFFFF,
        .seg_select = sel << 3,
        .reserved = 0,
        .reserved2 = 0,
        .gate_type = istrap ? IDT32_GATE_TRAP32 : IDT32_GATE_IRQ32,
        .priviledge = priviledge,
        .present = true,
    };
    idt_entries[idtno] = updated;
}

int idt_register_irq_handler(uint8_t irqno, struct irq_handler* hand)
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


    list_add_tail(&ident->list, &hand->listentry);

    if (enable && kinrange(irqno, PIC8259_IRQ0, PIC8259_IRQ0 + 16)) {
        pic8259_enable_irq(irqno - PIC8259_IRQ0);
    }

// restore_flags:
    irq_restore(flags);
    return err;
}
