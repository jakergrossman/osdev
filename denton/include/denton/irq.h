#ifndef __DENTON_IRQ_H
#define __DENTON_IRQ_H

#include <asm/irq.h>

int irq_register_handler(const char* name, int irqno, irqfn_t irqfn,
						 enum irq_type type, void* privdata);

#endif
