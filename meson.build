project('denton', 'c',
  version : '0.1',
  meson_version : '>=1.4',
  default_options : ['c_std=gnu99', 'warning_level=1', 'optimization=0'])

env = environment()
env.set('DESTDIR', 'sysroot')

add_global_arguments('-Wno-pedantic', '-ffreestanding', '-nostdlib', language : 'c')
add_global_link_arguments('-ffreestanding', '-nostdlib', language : 'c')

subdir('src')
