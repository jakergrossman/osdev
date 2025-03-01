#include <stdio.h>

#include <denton/bits.h>
#include <denton/tty.h>
#include <denton/klog.h>

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

	klog_info("%c: 0x%08X", 'e', 0xCAFEBABE);
}
