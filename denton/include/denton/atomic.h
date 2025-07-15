#ifndef __DENTON_ATOMIC_H
#define __DENTON_ATOMIC_H

// TODO: other support
#include <asm/atomic.h>

typedef atomic32_t atomic_t;
#define ATOMIC_INIT(...) ATOMIC32_INIT(__VA_ARGS__)
#define atomic_get(...) atomic32_get(__VA_ARGS__)
#define atomic_set(...) atomic32_set(__VA_ARGS__)
#define atomic_sub(...) atomic32_sub(__VA_ARGS__)
#define atomic_xadd(...) atomic32_xadd(__VA_ARGS__)
#define atomic_sub_return(...) atomic32_sub_return(__VA_ARGS__)
#define atomic_add_return(...) atomic32_add_return(__VA_ARGS__)
#define atomic_inc(...) atomic32_inc(__VA_ARGS__)
#define atomic_dec(...) atomic32_dec(__VA_ARGS__)
#define atomic_inc_return(...) atomic32_inc_return(__VA_ARGS__)
#define atomic_dec_return(...) atomic32_dec_return(__VA_ARGS__)
#define atomic_dec_and_test(...) atomic32_dec_and_test(__VA_ARGS__)

#endif
