#include "asm/instr.h"
#include <denton/bits.h>
#include <denton/tty.h>
#include <denton/klog.h>
#include <denton/list.h>
#include <denton/compiler.h>

#include <asm/irq.h>

#include <limits.h>

void kmain(void)
{
	// for now, update terminal base now that we are using the kernel pgdir
	terminal_update_base(INIT_VGA);

	volatile uint32_t n = *(uint32_t*)0xC0000001;


	irq_enable();

	asm volatile ("int $3");
	klog_info("OS is running...\n");
}
