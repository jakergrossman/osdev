#include "asm/instr.h"
#include <denton/bits.h>
#include <denton/tty.h>
#include <denton/klog.h>
#include <denton/list.h>
#include <denton/compiler.h>

#include <limits.h>
#include <stdlib.h>

void __naked int_handler(void) 
{
}
    // INIT_VGA = 0xC00B8000, // VGA mem after cmain

void kmain(void)
{
	// for now, update terminal base now that we are using the kernel pgdir
	terminal_update_base(INIT_VGA);

	asm volatile (
		"mov $0x123abc, 0xC0000000\n"
		"int $0xFF"
	);

	for(;;) {
		hlt();
	}
}
