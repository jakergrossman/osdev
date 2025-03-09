#include <denton/klog.h>

#include <denton/compiler.h>
#include <denton/types.h>
#include <denton/tty.h>
#include <denton/spinlock.h>

#include <asm/paging.h>
#include <asm/timer.h>

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

/* reserve 1 page for early klogging
 * once we are further along we will
 * migrate to some kzalloced buffers
 */
static uint8_t __boot_klog[PAGE_SIZE] __align(PAGE_SIZE);
static size_t  __boot_pos = 0;

static LIST_HEAD(klog_outputs);
static SPIN_LOCK(klog_output_lock, 0);

static void klog_write(struct klog_sink* sink, const char* buf, size_t buflen)
{
	// temporary, but we really just want to shove this in the circular buffer
	// it's other modules responsibilites to consume it...
	terminal_writestring(buf);
	terminal_flush();

	if ((sizeof(__boot_klog) - __boot_pos) >= buflen) {
		size_t back_half = sizeof(__boot_klog) - __boot_pos;
		size_t copy_len = kmin(buflen, back_half);

		memcpy(__boot_klog + __boot_pos, buf, copy_len);
		__boot_pos = (__boot_pos + copy_len) % sizeof(__boot_klog);

		buf += copy_len;
		buflen -= copy_len;
	}
	memcpy(__boot_klog + __boot_pos, buf, buflen);
	__boot_pos = (__boot_pos + buflen) % sizeof(__boot_klog);
}

static struct klog_sink klog_recorder = {
	.name = "klog",
	.list = LIST_HEAD_INIT(klog_recorder.list),
	.threshold = KLOG_TRACE,
	.write = klog_write,
};

void __klog(const char* funcname, enum klog_level level, const char* fmt, ...)
{
	uint32_t millis = timer_get_ms();
	uint32_t secs = millis / 1000;
	millis %= 1000;

	char buf[1024];
	size_t prefix_len = snprintf(buf, sizeof(buf), "[ %4d.%03d ][%s]: ", secs, millis, funcname);

	va_list args;
	va_start(args, fmt);
	size_t rest_len = vsnprintf(buf+prefix_len, sizeof(buf)-prefix_len, fmt, args);
	va_end(args);

	using_spin_lock(&klog_output_lock) {
		struct klog_sink* sink = NULL;
		struct klog_sink* tmp = NULL;
		list_for_each_entry_safe(&klog_outputs, sink, tmp, list) {
			if (level > sink->threshold) {
				continue;
			}

			sink->write(sink, buf, prefix_len + rest_len);
		}
	}
}

void klog_sink_register(struct klog_sink* sink)
{
	using_spin_lock(&klog_output_lock) {
		if (!list_placed_in_list(&sink->list)) {
			list_add_tail(&sink->list, &klog_outputs);
		}
	}
}

void klog_sink_deregister(struct klog_sink* sink)
{
	using_spin_lock(&klog_output_lock) {
		list_del(&sink->list);
	}
}

int klog_init(void)
{
	klog_sink_register(&klog_recorder);
	return 0;
}
