#ifndef __DENTON_COMPILER_H
#define __DENTON_COMPILER_H

#include <sys/compiler.h>

#define __kernel        __atr(address_space(0))
#define __user          __atr((noderef, address_space(__user)))
#define __iomem         __atr((noderef, address_space(__iomem)))
#define __percpu        __atr((noderef, address_space(__percpu)))

#endif
