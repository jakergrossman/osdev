#include "asm/timer.h"
#include "denton/heap.h"
#include "denton/mm/pga.h"
#include "denton/panic.h"
#include <limits.h>
#include <denton/bits.h>
#include <denton/tty.h>
#include <denton/klog.h>
#include <denton/list.h>

/**
 * paging setup on entry to kmain:
 * - enabled
 * - kernel mapped at 0xC0000000
 * - top-level page directory[-1] mapped to itself 
 */
void kmain(void)
{
	// for now, update terminal base now that we are using the kernel pgdir
	terminal_update_base(INIT_VGA);
	while(1) {
		for (int i = 0; i < 80; i++) {
			printf("%c", (rand() > INT_MAX/2) ? '@' : '.');
		}
		terminal_flush();
		for (volatile int i = 0; i < 0xFFFFFF; i++);
	}
}
