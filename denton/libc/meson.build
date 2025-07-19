subdir('include')
libc_c_srcs = []

subdir('stdio')
libc_c_srcs += libc_c_stdio_srcs

subdir('stdlib')
libc_c_srcs += libc_c_stdlib_srcs

subdir('string')
libc_c_srcs += libc_c_string_srcs

subdir('arch')

libk = static_library('k',
  libc_c_srcs,
  libc_arch_c_srcs,
  include_directories : [ libc_inc_dirs, kernel_inc_dirs ],
  c_args : [ '-D__is_libk', '-ffreestanding' ],
)

libc = static_library('c',
  libc_c_srcs,
  libc_arch_c_srcs,
  include_directories : [ libc_inc_dirs ],
  c_args : [ '-D__is_libc', '-ffreestanding' ],
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
