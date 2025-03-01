#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

size_t __itoa_len(long value, int radix)
{
    size_t len = 0;
    if (value < 0) {
        len += 1; // negative sign
        value = -value;
    }

    do {
        len += 1;
        value /= radix;
    } while (value > 0);

    return len;
}

int __itoa(
    long value,
    char* buf,
    size_t maxlen,
    unsigned int radix,
    bool is_upper,
    bool is_signed,
    int padlen
)
{
    if (maxlen == 0) {
        return 0;
    }

    /* no support for weird radixes */
    if (radix > 16) {
        return 0;
    }

    size_t val_len = __itoa_len(value, radix);
    if (val_len > maxlen) {
        return 0;
    }

    bool is_negative = false;
    if (value < 0 && is_signed) {
        is_negative = true;
        value = -value;
    }

    const char * const end = buf + maxlen;

    /* build the string back to front */
    const int hex_base = (is_upper ? 'A' : 'a') - 10;
    const int dec_base = '0';
    char* print_buffer = buf;
    do {
        int digit = value % radix;
        if (buf) {
            *print_buffer = digit + ((digit < 10) ? dec_base : hex_base);
            print_buffer++;
        }
        value /= radix;
    } while ((value > 0 || padlen) && (!buf || (print_buffer < end)));

    if (is_negative) {
        *print_buffer++ = '-';
    }

    *print_buffer = '\0';

    /* now we reverse it */
    size_t len = print_buffer - buf;
    for (size_t i = 0; i < len / 2; i++) {
        int front = buf[i];
        buf[i] = buf[len-i-1];
        buf[len-i-1] = front;
    }

    return len;
}

int itoa_n(long value, unsigned int radix, bool isupper, bool is_signed, char* buf, size_t maxlen)
{
    if (maxlen == 0) {
        return 0;
    }

    /* no support for weird radixes */
    if (radix > 16) {
        return 0;
    }

    bool is_negative = false;
    if (value < 0 && is_signed) {
        is_negative = true;
        value = -value;
    }

    const char * const end = buf + maxlen;

    /* build the string back to front */
    const int hex_base = (isupper ? 'A' : 'a') - 10;
    const int dec_base = '0';
    char* print_buffer = buf;
    do {
        int digit = value % radix;
        if (buf) {
            *print_buffer = digit + ((digit < 10) ? dec_base : hex_base);
            print_buffer++;
        }
        value /= radix;
    } while (value > 0 && (!buf || (print_buffer < end)));

    if (is_negative) {
        *print_buffer++ = '-';
    }

    *print_buffer = '\0';

    /* now we reverse it */
    size_t len = print_buffer - buf;
    for (size_t i = 0; i < len / 2; i++) {
        int front = buf[i];
        buf[i] = buf[len-i-1];
        buf[len-i-1] = front;
    }

    return len;
}

static bool print(const char* data, size_t length) {
    const unsigned char* bytes = (const unsigned char*) data;
    for (size_t i = 0; i < length; i++)
        if (putchar(bytes[i]) == EOF)
            return false;
    return true;
}

struct fmt_spec {
    char fmt;
    char pad;
    int padsize;
};

int printf_seek(const char* restrict fmt, char** endptr, struct fmt_spec* spec)
{
    if (!fmt) {
        return EOF;
    }

    char* ptr = (char*)fmt;
    int fmt_distance = EOF;
    while (ptr && *ptr) {
        if (*ptr == '%') {
            fmt_distance = ptr - fmt;
            ptr++;
            if ('0' == *ptr) {
                spec->pad = '0';
                ptr++;
            } else {
                spec->pad = ' ';
            }

            if (isdigit(*ptr) || (*ptr == '-')) {
                char* padend = NULL;
                spec->padsize = strtol(ptr, &padend, 10);
                ptr = padend;
            } else {
                spec->padsize = 0;
            }

            spec->fmt = *ptr++;
            *endptr = (char*)ptr;
            return fmt_distance;
        }
        ptr++;
    }


    if (*ptr == '\0') {
        *endptr = NULL;
    } else {
        *endptr = ptr;
    }

    return fmt_distance;
}

int printf_(const char* restrict format, ...)
{
    va_list parameters;
    va_start(parameters, format);

    struct fmt_spec spec;
    char* ptr = (char*)format;
    int written = 0;
    do {
        size_t maxrem = INT_MAX - written;
        char* endptr;
        int ret = printf_seek(ptr, &endptr, &spec);
        if (ret == EOF) {
            break;
        } else if (ret < 0) {
            // TODO: set errno
            return -1;
        }

        printf("spec: fmt: %c, pad: '%c', padsize: %d\n",
                spec.fmt, spec.pad, spec.padsize);

        switch (spec.fmt) {
            case 'd':
            case 'u':
            case 'x':
            case 'X':
                {
                    bool is_upper = (*format == 'X');
                    bool is_signed = *format == 'd';
                    int base = (tolower(*format) == 'x') ? 16 : 10;
                    format++;
                    static char fmtbuf[32];
                    int i = va_arg(parameters, int);
                    if (!maxrem) {
                        // TODO: Set errno to EOVERFLOW
                        return -1;
                    }
                    size_t size = itoa_n(i, base, is_upper, is_signed, fmtbuf, sizeof(fmtbuf));
                    if (size > maxrem) {
                        // TODO: set errno to EOVERFLOW
                        return -1;
                    }
                    print(fmtbuf, size);
                    written += size;
                }
                break;
        }

        ptr = endptr;
    } while (ptr);

    va_end(parameters);

    return 0;
}


int printf(const char* restrict format, ...) {
    va_list parameters;
    va_start(parameters, format);

    int written = 0;

    while (*format != '\0') {
        size_t maxrem = INT_MAX - written;

        if (format[0] != '%' || format[1] == '%') {
            if (format[0] == '%')
                format++;
            size_t amount = 1;
            while (format[amount] && format[amount] != '%')
                amount++;
            if (maxrem < amount) {
                // TODO: Set errno to EOVERFLOW.
                return -1;
            }
            if (!print(format, amount))
                return -1;
            format += amount;
            written += amount;
            continue;
        }

        const char* format_begun_at = format++;
        int pad = 0;

        switch (*format) {
            case 'c':
                {
                    format++;
                    char c = (char) va_arg(parameters, int /* char promotes to int */);
                    if (!maxrem) {
                        // TODO: Set errno to EOVERFLOW.
                        return -1;
                    }
                    if (!print(&c, sizeof(c))) {
                        return -1;
                    }
                    written++;
                }
                break;

            case 's':
                {
                    format++;
                    const char* str = va_arg(parameters, const char*);
                    size_t len = strlen(str);
                    if (maxrem < len) {
                        // TODO: Set errno to EOVERFLOW.
                        return -1;
                    }
                    if (!print(str, len)) {
                        return -1;
                    }
                    written += len;
                }
                break;

            case 'd':
            case 'u':
            case 'x':
            case 'X':
                {
                    bool is_upper = (*format == 'X');
                    bool is_signed = *format == 'd';
                    int base = (tolower(*format) == 'x') ? 16 : 10;
                    format++;
                    static char fmtbuf[64];
                    int i = va_arg(parameters, long);
                    if (!maxrem) {
                        // TODO: Set errno to EOVERFLOW
                        return -1;
                    }
                    size_t size = itoa_n(i, base, is_upper, is_signed, fmtbuf, sizeof(fmtbuf));
                    if (size > maxrem) {
                        // TODO: set errno to EOVERFLOW
                        return -1;
                    }
                    print(fmtbuf, size);
                    written += size;
                }
                break;

            default:
                {
                    format = format_begun_at;
                    size_t len = strlen(format);
                    if (maxrem < len) {
                        // TODO: Set errno to EOVERFLOW.
                        return -1;
                    }
                    if (!print(format, len))
                        return -1;
                    written += len;
                    format += len;
                }
                break;
        }
    }

    va_end(parameters);
    return written;
}
