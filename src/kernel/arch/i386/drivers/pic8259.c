#include "asm/atomic.h"
#include "asm/cpu.h"
#include "denton/bits/bits.h"
#include "denton/panic.h"
#include "denton/sched.h"
#include <denton/klog.h>
#include <denton/spinlock.h>
#include <asm/drivers/pic8259.h>

#include <asm/irq.h>
#include <asm/timer.h>
#include <asm/instr.h>

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
static spinlock_t irqmask_lock = SPINLOCK_INIT(0);
static atomic32_t system_tick = ATOMIC32_INIT(0);

static void pic_master_set_mask(void)
{
	outb(irqmask & 0xFF, PIC8259_IO_PIC1 + 1);
}

static void pic_slave_set_mask(void)
{
	outb(irqmask >> 8, PIC8259_IO_PIC2 + 1);
}

static void timer_callback(struct irq_frame* frame, void* privdata)
{
	atomic32_inc(&system_tick);
	cpu_get_local()->reschedule = true;
}

void pic8259_init(void)
{
	/* remap PIC1 to IRQ32 and PIC2 at IRQ40 */
	outb(0xFF, PIC8259_IO_PIC1 + 1);
	outb(0xFF, PIC8259_IO_PIC2 + 1);

	// restart PIC
	outb(0x11, PIC8259_IO_PIC1);
	outb(0x11, PIC8259_IO_PIC2);

	// PIC1 now starts at PIC8259_IRQ0
	outb(PIC8259_IRQ0, PIC8259_IO_PIC1 + 1);
	// PIC2 now starts at PIC8259_IRQ8
	outb(PIC8259_IRQ0 + 8, PIC8259_IO_PIC2 + 1);

	// setup cascading
	outb(1 << PIC8259_IRQ_SLAVE, PIC8259_IO_PIC1 + 1);
	outb(PIC8259_IRQ_SLAVE, PIC8259_IO_PIC2 + 1);

	outb(0x01, PIC8259_IO_PIC1 + 1);
	outb(0x01, PIC8259_IO_PIC2 + 1); // done!

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
	outb(PIC8259_TIMER_DIV(TIMER_TICKS_PER_SECOND) % 256, PIC8259_TIMER_IOBASE);
	outb(PIC8259_TIMER_DIV(TIMER_TICKS_PER_SECOND) / 256, PIC8259_TIMER_IOBASE);

	int err = irq_register_handler(
			"systick", 0, timer_callback, IRQ_INTERRUPT, 0);
	if (err) {
		panic();
	}
}

void pic8259_enable_irq(int irqno)
{
	spin_lock(&irqmask_lock);

	irqmask &= ~BIT(irqno);
	pic_master_set_mask();
	pic_slave_set_mask();

	spin_unlock(&irqmask_lock);
}

void pic8259_disable_irq(int irqno)
{
	spin_lock(&irqmask_lock);

	irqmask |= BIT(irqno);
	pic_master_set_mask();
	pic_slave_set_mask();

	spin_unlock(&irqmask_lock);
}

static uint8_t
pic8259_read_master_isr(void)
{
	outb(PIC8259_READ_ISR, PIC8259_IO_PIC1);
	return inb(PIC8259_IO_PIC1);
}

static uint8_t
pic8259_read_slave_isr(void)
{
	outb(PIC8259_READ_ISR, PIC8259_IO_PIC2);
	return inb(PIC8259_IO_PIC2);
}


void pic8259_send_eoi(int irq)
{
	spin_lock(&irqmask_lock);
	if (irq >= 8) {

		uint8_t isr = pic8259_read_slave_isr();
		uint8_t bit = BIT(irq - 8);

		if (isr & bit) {
			outb(PIC8259_EOI, PIC8259_IO_PIC2);
		}

		outb(PIC8259_EOI, PIC8259_IO_PIC1);
	} else {
		uint8_t isr = pic8259_read_master_isr();
		uint8_t bit = BIT(irq);

		if (isr & bit) {
			outb(PIC8259_EOI, PIC8259_IO_PIC1);
		}
	}
	spin_unlock(&irqmask_lock);
}

uint32_t pic8259_get_ticks(void)
{
	return atomic32_get(&system_tick);
}
