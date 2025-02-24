subdir('arch')
subdir('kernel')

cc = meson.get_compiler('c')
libgcc = cc.find_library('gcc')

kernel_image = 'kernel.bin'
kernel_sysroot_dir = '/boot'

kernel = executable(kernel_image,
  kernel_c_sources,
  kernel_arch_c_sources,
  include_directories : kernel_inc_dirs,
  link_args : [ '-T' + kernel_arch_linker_script.full_path() ],
  link_depends : kernel_arch_linker_script.full_path(),
  dependencies : [ libk_dep, libgcc ],

  install: true,
  install_dir: kernel_sysroot_dir,
)

kernel_dep = declare_dependency(include_directories : kernel_inc_dirs)
