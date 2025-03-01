subdir('denton')
subdir('arch')

cc = meson.get_compiler('c')
libgcc = cc.find_library('gcc')

kernel_image = 'denton.bin'
kernel_sysroot_dir = '/boot'

kernel = executable(kernel_image,
  kernel_c_sources,
  kernel_arch_c_sources,
  include_directories : [ kernel_inc_dirs, kernel_arch_inc_dirs ],
  link_args : [ '-T' + kernel_arch_linker_script.full_path(), '-D__LINKER__' ],
  link_depends : kernel_arch_linker_script,
  dependencies : [ libk_dep, libgcc ],

  install: true,
  install_dir: kernel_sysroot_dir,
)

kernel_dep = declare_dependency(include_directories : kernel_inc_dirs)
