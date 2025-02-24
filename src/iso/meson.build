conf = configuration_data()
conf.set('KERNEL_IMAGE_PATH', join_paths(kernel_sysroot_dir, kernel_image))

grub = configure_file(
  input: 'grub.cfg.in',
  output: 'grub.cfg',
  configuration: conf,
  install: true,
  install_dir: '/boot/grub',
)
