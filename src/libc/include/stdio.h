#ifndef _STDIO_H
#define _STDIO_H 1

#include <stdarg.h>
#include <sys/cdefs.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int printf(const char* __restrict, ...);
int sprintf(char* buffer, const char* format, ...);
int snprintf(char* buffer, size_t count, const char* format, ...);
int vprintf(const char* format, va_list va);
int vsnprintf(char* buffer, size_t count, const char* format, va_list va);
int putchar(int);
int puts(const char*);

#ifdef __cplusplus
}
#endif

#endif
