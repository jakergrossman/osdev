#include <asm/timer.h>
#include <asm/drivers/pic8259.h>

uint32_t timer_get_ms(void)
{
	return 1000 * timer_get_ticks() / TIMER_TICKS_PER_SECOND;
}

uint32_t timer_get_ticks(void)
{
	return pic8259_get_ticks();
}
