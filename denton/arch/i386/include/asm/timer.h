#ifndef __DENTON_1386_ASM_TIMER_H
#define __DENTON_1386_ASM_TIMER_H

#include <asm/drivers/pic8259.h>

#include <stdint.h>

#define TIMER_TICKS_PER_SECOND PIC8259_TICKS_PER_SEC

uint32_t timer_get_ms(void);
uint32_t timer_get_ticks(void);

#endif
