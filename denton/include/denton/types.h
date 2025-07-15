#ifndef __DENTON_TYPES_H
#define __DENTON_TYPES_H

#include <asm/types.h>
#include <denton/list.h>
#include <denton/math.h>

#include <stdbool.h>

typedef __kphysaddr_t physaddr_t;
typedef __kvirtaddr_t virtaddr_t;

#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof((arr)[0]))

#define asm_inline asm __inline

#endif
