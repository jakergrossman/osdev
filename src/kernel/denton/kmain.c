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

	struct page* p1 = page_alloc(8, 0);
	struct page* p2 = page_alloc(8, 0);
	page_free(p1+4, 4);
	struct page* p3 = page_alloc(8, 0);
	struct page* p4 = page_alloc(4, 0);

	// terminal_clear();
	klog_info("P1Virtual: 0x%08X\n", p1->virt);
	klog_info("P2Virtual: 0x%08X\n", p2->virt);
	klog_info("P3Virtual: 0x%08X\n", p3->virt);
	klog_info("P4Virtual: 0x%08X\n", p4->virt);
}
