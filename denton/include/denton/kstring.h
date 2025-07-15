#ifndef __DENTON_KSTRING_H
#define __DENTON_KSTRING_H

#include <stdio.h>
#include <string.h>
#include <denton/heap.h>

static inline char*
kstrdup(const char* orig, unsigned int flags)
{
	size_t len = strlen(orig);
	char* newmem = kmalloc(len, flags);
	if (!newmem) {
		return newmem;
	}
	memcpy(newmem, orig, len+1);
	printf("new: '%s'\n", newmem);
	return newmem;
}

#endif
