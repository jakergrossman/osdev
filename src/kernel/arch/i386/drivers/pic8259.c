#include "denton/bits/bits.h"
#include "denton/panic.h"
#include <denton/klog.h>
#include <denton/atomic.h>
#include <asm/drivers/pic8259.h>

#include <asm/irq.h>
#include <asm/timer.h>
#include <asm/instr.h>

static atomic32_t system_tick;

enum pic8259_register {
	PIC8259_TIMER_IOBASE = 0x40,
	PIC8259_TIMER_MODE_REG = PIC8259_TIMER_IOBASE + 3,
#define PIC8259_TIMER_SEL0	 0x00
#define PIC8259_TIMER_RATGEN 0x04
#define PIC8259_TIMER_16BIT  0x30
};

#define PIC8259_TIMER_FREQ		1193182
#define PIC8259_TIMER_DIV(n)	(PIC8259_TIMER_FREQ / (n))

#define PIC8259_IO_PIC1  0x20
#define PIC8259_IO_PIC2  0xA0
#define PIC8259_IRQ_SLAVE 2
#define PIC8259_EOI 0x20
#define PIC8259_READ_ISR 0x0B

static uint16_t irqmask = 0xFFF & ~(1UL<<PIC8259_IRQ_SLAVE);

static void pic_master_set_mask(void)
{
	outb(PIC8259_IO_PIC1 + 1, irqmask & 0xFF);
}

static void pic_slave_set_mask(void)
{
	outb(PIC8259_IO_PIC2 + 1, irqmask >> 8);
}

static void timer_callback(struct irq_frame* frame, void* privdata)
{
	atomic32_inc(&system_tick);
}

void pic8259_init(void)
{
	outb(PIC8259_IO_PIC1 + 1, 0xFF);
	outb(PIC8259_IO_PIC2 + 1, 0xFF);

	outb(PIC8259_IO_PIC1, 0x11);
	outb(PIC8259_IO_PIC2, 0x11);

	outb(PIC8259_IO_PIC1 + 1, PIC8259_IRQ0);
	outb(PIC8259_IO_PIC2 + 1, PIC8259_IRQ0 + 8);

	outb(PIC8259_IO_PIC1 + 1, 1 << PIC8259_IRQ_SLAVE);
	outb(PIC8259_IO_PIC2 + 1, PIC8259_IRQ_SLAVE);

	outb(PIC8259_IO_PIC1 + 1, 0x01);
	outb(PIC8259_IO_PIC2 + 1, 0x01);

	pic_master_set_mask();
	pic_slave_set_mask();
}

void pic8259_timer_init(void)
{
	klog_trace("setting pic8259 (%d Hz)...\n", TIMER_TICKS_PER_SECOND);
	outb(PIC8259_TIMER_MODE_REG,
		PIC8259_TIMER_SEL0 |
		PIC8259_TIMER_RATGEN |
		PIC8259_TIMER_16BIT
	);
	outb(PIC8259_TIMER_IOBASE, PIC8259_TIMER_DIV(TIMER_TICKS_PER_SECOND) % 256);
	outb(PIC8259_TIMER_IOBASE, PIC8259_TIMER_DIV(TIMER_TICKS_PER_SECOND) / 256);

	int err = irq_register_handler(
			"systick", 0, timer_callback, IRQ_INTERRUPT, 0);
	if (err) {
		panic();
	}
}

void pic8259_enable_irq(int irqno)
{
	// TODO: spinlock
	irqmask &= ~BIT(irqno);
	pic_master_set_mask();
	pic_slave_set_mask();
}

void pic8259_disable_irq(int irqno)
{
	// TODO: spinlock
	irqmask |= BIT(irqno);
	pic_master_set_mask();
	pic_slave_set_mask();
}

static uint8_t
pic8259_read_master_isr(void)
{
	outb(PIC8259_IO_PIC1, PIC8259_READ_ISR);
	return inb(PIC8259_IO_PIC1);
}

static uint8_t
pic8259_read_slave_isr(void)
{
	outb(PIC8259_IO_PIC2, PIC8259_READ_ISR);
	return inb(PIC8259_IO_PIC2);
}


void pic8259_send_eoi(int irq)
{
	// TODO: spinlock
	if (irq >= 8) {
		uint8_t isr = pic8259_read_slave_isr();
		uint8_t bit = BIT(isr - 8);

		if (isr & bit) {
			outb(PIC8259_IO_PIC2, PIC8259_EOI);
		}

		outb(PIC8259_IO_PIC1, PIC8259_EOI);
	} else {
		uint8_t isr = pic8259_read_master_isr();
		uint8_t bit = BIT(isr);

		if (isr & bit) {
			outb(PIC8259_IO_PIC1, PIC8259_EOI);
		}
	}
}

uint32_t pic8259_get_ticks(void)
{
	return atomic32_get(&system_tick);
}
