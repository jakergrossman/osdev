kernel_c_sources = files(
  'init.c',
  'kmain.c',
  'heap.c',
  'kparam.c',
  'irq.c',
  'ring.c',
  'klog.c',
  'bma.c',
  'pci.c',
  'disk/ata.c',
)

subdir('mm')
kernel_c_sources += kernel_c_mm_sources

subdir('sched')
kernel_c_sources += kernel_c_sched_sources

subdir('sync')
kernel_c_sources += kernel_c_sync_sources
