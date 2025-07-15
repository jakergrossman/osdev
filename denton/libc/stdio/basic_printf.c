#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


static void printf_str()
{
}

static void printf_int()
{
}

int basic_vsnprintf(char* buf, size_t buflen, const char* fmt, va_list arg)
{
	size_t fmtlen = strlen(fmt);

	const char* fmt_ptr = fmt;
	char* end = (char*)&fmt[0];
	struct fmt_spec spec;

	int total = 0;

	while (fmt_seek(fmt_ptr, fmtlen - (end - fmt), &end, &spec) != EOF) {
		int maxrem = (int)buflen - total;
		size_t skip = (end -fmt_ptr);
		if (maxrem > 0) {
			size_t count = (skip > maxrem) ? maxrem : skip;
			memcpy(&buf[total], fmt_ptr, count);
			total += skip;
			maxrem -= total;
		}

		switch (spec.ch) {
			case 'c':
				if (maxrem > 0) {
					int ch = va_arg(arg, int);
					*buf++ = toascii(ch);
				}
				total++;
				maxrem--;
				break;
			case 's':
				if (maxrem > 0) {
					size_t count = (skip > maxrem) ? maxrem : skip;
					memcpy(&buf[total], fmt_ptr, count);
					total += skip;
					maxrem -= total;
				}
				break;
		}

		// printf("ch: %c\n", spec.ch);
		// printf("justify: %d, %s\n", spec.width, spec.left_justify ? "left" : "right");
		// printf("fmtnum: %d\n", spec.fmt_num);
		// printf("remanining: '%s'\n\n", end);
		fmt_ptr = end;
	}

	if (*fmt_ptr != '0') {
		size_t rem = buflen - (fmt_ptr - fmt);
	}

	return total;
}
