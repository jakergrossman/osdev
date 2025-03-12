#ifndef __DENTON_DRIVERS_PIC8259_H
#define __DENTON_DRIVERS_PIC8259_H

#include <stdint.h>

#define PIC8259_IRQ0			0x20
#define PIC8259_TICKS_PER_SEC	250

void pic8259_init(void);
void pic8259_timer_init(void);
void pic8259_enable_irq(int irqno);
void pic8259_disable_irq(int irqno);
uint32_t pic8259_get_ticks(void);
void pic8259_send_eoi(int irqno);

#endif
