project('myos', 'c',
  version : '0.1',
  meson_version : '>=1.4',
  default_options : ['c_std=gnu99', 'warning_level=3', 'optimization=2'])

add_global_arguments('-ffreestanding', '-nostdlib', language : 'c')
add_global_link_arguments('-ffreestanding', '-nostdlib', language : 'c')

subdir('src/kernel/include')
subdir('src/libc/include')

subdir('src/libc')
subdir('src/kernel')
