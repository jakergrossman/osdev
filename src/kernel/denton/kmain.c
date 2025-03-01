#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <limits.h>
#include <stdbool.h>

#include <denton/bits.h>
#include <denton/tty.h>
#include <denton/compiler.h>
#include <denton/klog.h>


// struct fmt_spec {
// 	char fmt;
// 	char pad;
// 	int padsize;
// };

// static bool print(const char* data, size_t length) {
// 	const unsigned char* bytes = (const unsigned char*) data;
// 	for (size_t i = 0; i < length; i++)
// 		if (putchar(bytes[i]) == EOF)
// 			return false;
// 	return true;
// }

// int printf_seek(const char* restrict fmt, char** endptr, struct fmt_spec* spec);

// int printf__(const char* fmt, ...) {
// 	va_list args;
// 	va_start(args, fmt);

// 	char *ptr = fmt;
// 	char *endptr = NULL;

// 	struct fmt_spec spec;
// 	size_t written = 0;
// 	size_t fmt_distance = 0;

// 	va_end(args);
// 	int fmt_written;
// 	while ((fmt_written = printf_seek(ptr, &endptr, &spec)) != EOF) {
// 		// printf("\nfmt: '%c', pad: '%c', padsize: %d\n", spec.fmt, spec.pad, spec.padsize);
// 		if (!print(ptr, fmt_written)) {
// 			return -1;
// 		}

// 		size_t max_rem = INT_MAX - written;
// 		switch (spec.fmt) {
// 			case 's':
// 				{
// 					if (!max_rem) {
// 						// TODO: set errno to EOVERFLOW
// 						return -1;
// 					}

// 					const char* str = va_arg(args, const char*);
// 					size_t len = strlen(str);
// 					if (spec.padsize < 0) {
// 						// right pad
// 						spec.padsize *= -1;
// 						size_t pad = len >= spec.padsize ? 0 : spec.padsize-len;
// 						if (!print(str, len)) {
// 							return -1;
// 						}

// 						for (size_t i = 0; i < pad; i++) {
// 							if (putchar(spec.pad) == EOF) {
// 								return -1;
// 							}
// 						}
// 					} else {
// 						// left pad
// 						size_t pad = len >= spec.padsize ? 0 : spec.padsize-len;
// 						for (size_t i = 0; i < pad; i++) {
// 							if (putchar(spec.pad) == EOF) {
// 								return -1;
// 							}
// 						}

// 						if (!print(str, len)) {
// 							return -1;
// 						}
// 					}

// 					written += len;
// 				}
// 				break;
// 		}
// 		ptr = endptr;
// 	}
// }

const char* homer = 
"   ___  _____    \n"
" .'/,-Y\"     \"~-.  \n"
" l.Y             ^.           \n"
" /\\               _\\_      \"Doh!\"   \n"
"i            ___/\"   \"\\ \n"
"|          /\"   \"\\   o !   \n"
"l         ]     o !__./   \n"
" \\ _  _    \\.___./    \"~\\  \n"
"  X \\/ \\            ___./  \n"
" ( \\ ___.   _..--~~\"   ~`-.  \n"
"  ` Z,--   /               \\    \n"
"    \\__.  (   /       ______) \n"
"      \\   l  /-----~~\" /      -Row\n"
"       Y   \\          / \n"
"       |    \"x______.^ \n"
"       |           \\    \n"
"       j            Y\n"
;

/**
 * paging setup on entry to kmain:
 * - enabled
 * - kernel mapped at 0xC0000000
 * - top-level page directory[-1] mapped to itself 
 */
void kmain(void)
{
	// for now, update terminal base now that we are using the kernel pgdir
	terminal_update_base(INIT_VGA);
	klog_trace("hello from kmain\n");
	for (int i = 1; i < 1024; i *= 2) {
		klog_debug("%d\n", i);
	}
}
