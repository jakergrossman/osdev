kernel_arch_inc_dirs = include_directories(
  'include',
)

kernel_arch_c_sources = files(
  'tty.c',
  'cpuid.c',
  'cpu.c',
  'gdt.c',
  'timer.c',
  'irq.c',
  'idt.c',
  'irq_handler.S',
  'task.S',
  'task.c',
  'sync/spinlock.c',
)

subdir('boot')
kernel_arch_c_sources += kernel_arch_boot_c_srcs

subdir('mm')
kernel_arch_c_sources += kernel_arch_mm_c_srcs

subdir('drivers')
kernel_arch_c_sources += kernel_arch_c_driver_sources
