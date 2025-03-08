#include "asm/instr.h"
#include "asm/timer.h"
#include <denton/bits.h>
#include <denton/tty.h>
#include <denton/klog.h>
#include <denton/list.h>
#include <denton/compiler.h>
#include <denton/ring.h>

#include <asm/irq.h>

#include <limits.h>

void kmain(void)
{
	// for now, update terminal base now that we are using the kernel pgdir
	terminal_update_base(INIT_VGA);

	klog_info("OS is running...\n");
	asm volatile (
		"	cli\n"
		"1: jmp 1b\n"
	);

	sti();

	uint32_t next = 0;
	while (1) {
		uint32_t ms = timer_get_ms();
		if (ms >= next)
		{
			klog_error("%ld\n", (long)ms);
			next = ms + 250;
		}
	}

	klog_error("aaaand its dead\n");
}
