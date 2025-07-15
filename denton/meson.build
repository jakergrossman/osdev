subdir('include')
subdir('libc')
subdir('arch')
subdir('kernel')

libgcc = cc.find_library('gcc')

kernel_image = 'denton.bin'
kernel_sysroot_dir = '/boot'
kernel_link_args = [ '-T' + kernel_arch_linker_script.full_path(), '-D__LINKER__' ]
kernel_include_dirs = [ kernel_inc_dirs, kernel_arch_inc_dirs, libc_inc_dirs ]

# most of the kernel, with initcalls stubbed out
kernel = static_library('kernel_obj',
                        kernel_c_sources,
                        kernel_arch_c_sources,
                        c_args : '--no-pie',
                        link_depends : kernel_arch_linker_script,
                        include_directories : kernel_include_dirs,
                        dependencies : [ libk_dep, libgcc ])

# generate initcalls by parsing the static library
initcall_c = custom_target('initcall.c',
                           output : 'initcall.c',
                           command : [script_initcall, kernel.full_path(), '@OUTPUT@'],
                           depends : kernel)

# version information baked in
version_c = custom_target('version.c',
                          output : 'version.c',
                          command : [script_gitrev, '@OUTPUT@'])


kernel_bin = executable(kernel_image,
                        initcall_c,
                        version_c,
                        include_directories: kernel_include_dirs,
                        link_whole : kernel,
                        link_args : kernel_link_args)

kernel_dep = declare_dependency(include_directories : kernel_inc_dirs)
