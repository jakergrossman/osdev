libd_srcs = files(
  'min_heap.c',
)

libd_inc_dirs = []

libd = library('d',
  libd_srcs,
  dependencies : [ libk_dep ],
  include_directories : [ libd_inc_dirs, kernel_inc_dirs ],
  c_args : [ '-D__is_libd', '--shared' ],
  pic : true,
  install : true,
)

libd_dep = declare_dependency(
  link_with : libd,
  include_directories : [ libd_inc_dirs ],
)
