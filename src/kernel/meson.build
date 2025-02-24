subdir('arch')
subdir('kernel')

cc = meson.get_compiler('c')
libgcc = cc.find_library('gcc')

kernel_image = 'kernel.bin'
kernel_sysroot_dir = '/boot'

inc = include_directories('include')
kernel = executable(kernel_image,
  kernel_c_sources,
  kernel_arch_c_sources,
  include_directories : inc,
  link_args : [ '-T' + kernel_arch_linker_script.full_path() ],
  link_depends : kernel_arch_linker_script.full_path(),
  dependencies : [ libk_dep, libgcc ],

  install: true,
  install_dir: '/boot',
)

kernel_dep = declare_dependency(include_directories : inc)
