cc = meson.get_compiler('c')
libgcc = cc.find_library('gcc')

linker_script = configure_file(input: 'linker.ld', output: 'linker.ld', copy: true)

bin = executable('myos.bin',
  'main.c',
  'boot.s',
  dependencies : libgcc,
  link_args : [ '-T', linker_script.full_path() ],
  link_depends : 'linker.ld'
)
