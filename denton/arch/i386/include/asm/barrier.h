#ifndef __DENTON_ARCH_I386_ASM_BARRIER_H
#define __DENTON_ARCH_I386_ASM_BARRIER_H

#define mb() asm volatile("lock; addl $0,-4(%%esp)" ::: "memory", "cc")
#define rmb() asm volatile("lock; addl $0,-4(%%esp)" ::: "memory", "cc")
#define wmb() asm volatile("lock; addl $0,-4(%%esp)" ::: "memory", "cc")

#define barrier() asm volatile("" : : : "memory")

#endif 
