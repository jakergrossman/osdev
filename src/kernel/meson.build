subdir('arch')
subdir('kernel')

cc = meson.get_compiler('c')
libgcc = cc.find_library('gcc')

linker_script = configure_file(
  input: kernel_arch_linker_script.full_path(),
  output: 'linker.ld',
  copy: true
)

inc = include_directories('include')
kernel = executable('kernel.bin',
  kernel_c_sources,
  kernel_arch_c_sources,
  include_directories : inc,
  link_args : [ '-T' + linker_script.full_path() ],
  link_depends : kernel_arch_linker_script.full_path(),
  dependencies : [ libk_dep, libgcc ],
)

kernel_dep = declare_dependency(include_directories : inc)
