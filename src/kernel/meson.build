subdir('denton')
subdir('arch')

cc = meson.get_compiler('c')
libgcc = cc.find_library('gcc')

kernel_image = 'denton.bin'
kernel_sysroot_dir = '/boot'
kernel_link_args = [ '-T' + kernel_arch_linker_script.full_path(), '-D__LINKER__' ]

# this is a little ugly. we need an object to parse the initcalls out of, but
# we can't use the full kernel, because it will try and link WITH the initcalls,
# which we don't have yet! so, we build a dummy library to pull out the initcalls,
# and use the objects from the library in the main compilation
kernel_dummy_lib = static_library('__kernel',
  kernel_c_sources,
  kernel_arch_c_sources,
  c_args : '--no-pie',
  link_args : kernel_link_args,
  link_depends : kernel_arch_linker_script,
  include_directories : [ kernel_inc_dirs, kernel_arch_inc_dirs ],
  dependencies : [ libk_dep, libgcc, libd_dep ],
)

initcall_c = custom_target('initcall.c',
  output : 'initcall.c',
  command : [script_initcall, kernel_dummy_lib.full_path(), '@OUTPUT@'],
  depends : kernel_dummy_lib,
)

version_c = custom_target('version.c',
  output : 'version.c',
  command : [script_gitrev, '@OUTPUT@'],
)

kernel = executable(kernel_image,
  initcall_c, version_c,
  objects : kernel_dummy_lib.extract_all_objects(recursive : true),
  link_args : kernel_link_args,
  link_depends : kernel_arch_linker_script,
  dependencies : [ libk_dep, libgcc ],
  include_directories : [ kernel_inc_dirs, kernel_arch_inc_dirs ],

  install: true,
  install_dir: kernel_sysroot_dir,
)

kernel_dep = declare_dependency(include_directories : kernel_inc_dirs)
