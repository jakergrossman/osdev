libc_c_srcs = files(
  'stdio/puts.c',
  'stdio/printf.c',
  'stdio/putchar.c',
  'stdlib/abort.c',
  'string/memcmp.c',
  'string/memcpy.c',
  'string/memmove.c',
  'string/memset.c',
  'string/strlen.c',
)

subdir('arch')

libk = static_library('k',
  libc_c_srcs,
  libc_arch_c_srcs,
  include_directories : [ libc_inc_dirs, kernel_inc_dirs ],
  c_args : [ '-D__is_libk', ],
)

libc = static_library('c',
  libc_c_srcs,
  libc_arch_c_srcs,
  include_directories : [ libc_inc_dirs ],
  c_args : [ '-D__is_libc', ],
  install : true,
  install_tag : 'sysroot',
)

libk_dep = declare_dependency(
  link_with : libk,
  include_directories : [ libc_inc_dirs ]
)

libc_dep = declare_dependency(
  link_with : libc,
  include_directories : [ libc_inc_dirs ]
)
