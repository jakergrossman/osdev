cc = meson.get_compiler('c')

kernel_arch_inc_dirs = include_directories(
  'include'
)

kernel_arch_c_sources = files(
  'boot.S',
  'kernel.c',
)  

kernel_arch_linker_script = cc.preprocess('linker.ld.in',
                                          output: '@BASENAME@',
                                          include_directories : [ kernel_arch_inc_dirs, kernel_inc_dirs ],
                                          )[0]
