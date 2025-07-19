#ifndef _MEMLAYOUT_H
#define _MEMLAYOUT_H

#include <denton/memsize.h>

//#define __KERNEL_VIRTBASE 0xC0000000
#define __KERNEL_VIRTBASE 0x0
#define __KERNEL_PHYSBASE __MiX(1)

#define V2PL(n) ((n) - __KERNEL_VIRTBASE)
#define P2VL(n) ((n) + __KERNEL_VIRTBASE)
#define V2P(n) ((physaddr_t)(n) & ~__KERNEL_VIRTBASE)
#define P2V(n) ((physaddr_t)(n) |  __KERNEL_VIRTBASE)
#define v_to_p(va) (physaddr_t)(V2P((uintptr_t)(va)))
#define p_to_v(pa) (virtaddr_t)(P2V((uintptr_t)(pa)))

#define virt_to_phys(x) ((uintptr_t)(x) - __KERNEL_VIRTBASE)
#define phys_to_virt(x) ((void*)((uintptr_t)(x) + __KERNEL_VIRTBASE))
#define virt_to_phys_wo(x) ((x) - __KERNEL_VIRTBASE)
#define phys_to_virt_wo(x) ((x) + __KERNEL_VIRTBASE)

#endif
