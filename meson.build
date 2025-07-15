project('denton', 'c',
  version : '0.1',
  meson_version : '>=1.7',
  default_options : ['c_std=gnu99', 'warning_level=1', 'optimization=0', 'default_library=static'])

env = environment()
env.set('DESTDIR', 'sysroot')

add_global_arguments('-Wno-pedantic', '-ffreestanding', '-nostdlib', language : 'c')
add_global_link_arguments('-ffreestanding', '-nostdlib', language : 'c')

subdir('scripts')
subdir('denton')
subdir('iso')
