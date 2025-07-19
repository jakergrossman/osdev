project('denton', 'c',
  version : '0.1',
  meson_version : '>=1.3',
  default_options : ['c_std=gnu99', 'warning_level=1', 'optimization=0', 'default_library=static'])

env = environment()
env.set('DESTDIR', 'sysroot')

subdir('scripts')
subdir('denton')
subdir('iso')
