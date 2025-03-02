#include <denton/bits.h>
#include <denton/tty.h>
#include <denton/klog.h>
#include <denton/list.h>
#include <denton/spinlock.h>

#include <limits.h>
#include <stdlib.h>

void kmain(void)
{
	// for now, update terminal base now that we are using the kernel pgdir
	terminal_update_base(INIT_VGA);

	spinlock_t lock = { 0 };

	spin_lock(&lock);

	klog_debug("i eat bugs, good night\n");

	spin_unlock(&lock);
}
