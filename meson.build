project('myos', 'c',
  version : '0.1',
  meson_version : '>=1.4',
  default_options : ['c_std=gnu99', 'warning_level=3', 'optimization=2'])

add_global_arguments('-ffreestanding', '-nostdlib', language : 'c')
add_global_link_arguments('-ffreestanding', '-nostdlib', language : 'c')


# cc = meson.get_compiler('c')
# libgcc = cc.find_library('gcc')

# bin = executable('myos.bin',
#   'kernel.c',
#   'boot.s',
#   dependencies : libgcc,
#   link_args : [ '-T', 'linker.ld' ],
#   link_depends : 'linker.ld'
# )

subdir('src')
