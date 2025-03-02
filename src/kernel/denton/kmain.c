#include "asm/timer.h"
#include "denton/heap.h"
#include "denton/mm/pga.h"
#include "denton/panic.h"
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
	klog_debug("timer: %d\n", timer_get_ticks());
}
