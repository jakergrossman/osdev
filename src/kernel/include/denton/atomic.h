#ifndef __DENTON_ATOMIC_H
#define __DENTON_ATOMIC_H

// TODO: other support
#include <asm/atomic.h>

typedef atomic32_t atomic_t;
#define atomic_get atomic32_get
#define atomic_set atomic32_set
#define atomic_set atomic32_set
#define atomic_sub atomic32_sub
#define atomic_xadd atomic32_xadd
#define atomic_sub_return atomic32_sub_return
#define atomic_add_return atomic32_add_return
#define atomic_inc atomic32_inc
#define atomic_dec atomic32_dec
#define atomic_inc_return atomic32_inc_return
#define atomic_dec_return atomic32_dec_return
#define atomic_dec_and_test atomic32_dec_and_test

#endif
