#include <denton/klog.h>
#include <asm/paging.h>

// TODO: fixme
extern char __kernel_cmdline[PAGE_SIZE];

void kernel_cmdline_init(const char* cmdline)
{
    klog(KLOG_INFO, "cmdline: %s\n", cmdline);
}
