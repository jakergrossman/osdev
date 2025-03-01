#ifndef __DENTON_KLOG_H
#define __DENTON_KLOG_H

#include <stdio.h>
#include <stdarg.h>

enum klog_level {
    KLOG_TRACE,
    KLOG_DEBUG,
    KLOG_INFO,
    KLOG_WARN,
    KLOG_ERROR,
    KLOG_EMERG,
};


#define klog(level, fmt, ...) \
    printf("[ 0.000000 ][ %s ] " fmt, __func__ __VA_OPT__(,) __VA_ARGS__)

#define klog_trace(...) klog(KLOG_TRACE, __VA_ARGS__)
#define klog_debug(...) klog(KLOG_DEBUG, __VA_ARGS__)
#define klog_info(...) klog(KLOG_INFO, __VA_ARGS__)
#define klog_warn(...) klog(KLOG_WARN, __VA_ARGS__)
#define klog_error(...) klog(KLOG_ERR, __VA_ARGS__)
#define klog_emerg(...) klog(KLOG_EMERG, __VA_ARGS__)

#endif
