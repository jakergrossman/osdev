#include "asm/instr.h"
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

	uint8_t buf[128];
	struct ringbuffer ring = RING_INIT(buf, sizeof(buf));

	for (uint8_t i = 0; i < 50; i++) {
		ring_put(&ring, &i, 1);
	}

	for (uint8_t i = 0; i < 25; i++) {
		uint8_t r;
		ring_get(&ring, &r, 1);
		printf("ring: %d ", r);
	}

	for (uint8_t i = 0; i < 75; i++) {
		ring_put(&ring, &i, 1);
	}

	uint8_t r;
	while (ring_get(&ring, &r, 1)) {
		printf("ring: %d ", r);
	}

	terminal_flush();


	irq_enable();
	klog_info("OS is running...\n");
}
