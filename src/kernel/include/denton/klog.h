#ifndef __DENTON_KLOG_H
#define __DENTON_KLOG_H

#include <denton/types.h>

#include <stdio.h>
#include <stdarg.h>

enum klog_level {
	KLOG_EMERG,
	KLOG_ERROR,
	KLOG_WARN,
	KLOG_INFO,
	KLOG_DEBUG,
	KLOG_TRACE,
};

enum klog_info_flag {
	KLOGF_FORCE_CONSOLE = 1,
	KLOGF_NEWLINE = 2,
	KLOGF_CONTINUE = 3,
};

struct klog_info {
	uint64_t        sequence;
	uint64_t        timestamp_ns;
	size_t          text_len;
	enum klog_level level;
	uint32_t        caller_token;
};

struct klog_record {
	struct klog_info info;
	char* buf;
	size_t bufsz;
};

/**
 * klog_sink : a recipient of klog
 * @name: the user-friendly sink name
 * @list: the entry into the klog sink list
 * @threshold: the lowest level that this sink will currently accept
 *
 * @write() - receive a klog entry on the sink
 * - @sink: the sink receiving the entry
 * - @buf: the formatted klog entry including prefix
 * - @buflen: the size of @buf, including the null terminator
 */
struct klog_sink {
	const char* name;
	struct list_head list;
	enum klog_level threshold;

	void (*write)(struct klog_sink* sink, const char* buf, size_t buflen);
};

/** register a recipient with klog */
void klog_sink_register(struct klog_sink* sink);
/** unregister a recipient with klog */
void klog_sink_deregister(struct klog_sink* sink);

/** do a klog */
void __klog(const char* funcname, enum klog_level level, const char* fmt, ...) __format(3, 4);



/* printf-style logging macro */
#define klog(level, fmt, ...) __klog(__func__, (level), (fmt) __VA_OPT__(,) __VA_ARGS__)

#define klog_trace(...) klog(KLOG_TRACE, __VA_ARGS__)
#define klog_debug(...) klog(KLOG_DEBUG, __VA_ARGS__)
#define klog_warn(...) klog(KLOG_WARN, __VA_ARGS__)
#define klog_error(...) klog(KLOG_ERROR, __VA_ARGS__)
#define klog_emerg(...) klog(KLOG_EMERG, __VA_ARGS__)

int klog_init(void);

#endif
