#include <denton/initcall.h>
#include <denton/klog.h>

static void init_early(void)
{
  klog_trace("on entry\n");
}
initcall(early, init_early);

static void init_core(void)
{
  klog_trace("on entry\n");
}
initcall(core, init_core);

static void init_subsystem(void)
{
  klog_trace("on entry\n");
}
initcall(subsystem, init_subsystem);

static void init_device(void)
{
  klog_trace("on entry\n");
}
initcall(device, init_device);

initcall_depends(core, early);
initcall_depends(subsystem, core);
initcall_depends(device, subsystem);
