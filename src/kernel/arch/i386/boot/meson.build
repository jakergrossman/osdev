cc = meson.get_compiler('c')

kernel_arch_boot_c_srcs = files(
  'boot_multiboot.S',
  'init.c',
  'crti.S',
  'crtn.S',
)

kernel_arch_linker_script = cc.preprocess('linker.ld.in',
  output: '@BASENAME@',
  include_directories : [ kernel_arch_inc_dirs, kernel_inc_dirs ],
)[0]
