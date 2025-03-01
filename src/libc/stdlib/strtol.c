#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

long strntol(const char* str, size_t len, char** endptr, int base)
{
	if (base > 16) {
		return -1;
	}

	bool is_negative = false;
	const char* ptr = str;
	if (*ptr == '-') {
		is_negative = true;
		ptr++;
	} else if (!isxdigit(*ptr)) {
		return 0;
	}

	if ((ptr - str) >= len) {
		*endptr = (char*)str + len;
		return -1;
	}

	static const char map[] = "01234567890abcdef";
	int acc = 0;
	while (((ptr - str) < len) && ptr && *ptr && isxdigit(*ptr)) {
		int digit = tolower(*ptr);
		int value = 0;
		while (map[value] != digit) {
			value++;
		}

		if (value >= base) {
			break;
		}

		acc *= base;
		acc += value;

		ptr++;
	}

	*endptr = (char*)ptr;
	return is_negative ? -acc : acc;
}

long strtol(const char* str, char** endptr, int base)
{
	return strntol(str, SIZE_MAX, endptr, base);
}

unsigned long strtoul(const char* str, char** endptr, int base);

unsigned int atoi(const char* str)
{
	return (int)strtol(str, NULL, 10);
}
