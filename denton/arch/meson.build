#TODO: more arches
if host_machine.cpu() == 'i386' or host_machine.cpu() == 'i686'
  subdir('i386')
elif host_machine.cpu() == 'aarch64'
  subdir('aarch64')
else
  assert(false, 'unsupported architecture: ' + host_machine.cpu())
endif
