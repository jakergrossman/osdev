#ifndef _CTYPE_H
#define _CTYPE_H

static inline int isalnum(int c)
{
	return (c >= '0' && c <= '9') ||
		   (c >= 'a' && c <= 'z') ||
		   (c >= 'A' && c <= 'Z');
}

static inline int isalpha(int c )
{
	return (c >= 'a' && c <= 'z') ||
		   (c >= 'A' && c <= 'Z');
}

static inline int isascii(int c)
{
	// if C is a 7-bit value
	return (c & ~0x7F) == 0;
}

static inline int iscntrl(int c)
{
	return (c == 0x80) || (c >= 0 && c <= 31);
}

static inline int isdigit(int c)
{
	return ((c >= '0') && (c <= '9'));
}

static inline int isprint(int c)
{
	return ((c >= 0x20) && (c <= 0x7E));
}

static inline int isgraph(int c)
{
	return isprint(c) && c != ' ';
}

static inline int islower(int c)
{
	return ((c >= 'a') && (c <= 'z'));
}


static inline int isspace(int c)
{
	switch (c) {
		case ' ':
		case '\t':
		case '\n':
		case '\r':
		case '\f':
		case '\v':
			return 1;
		default:
			return 0;
	}
}

static inline int ispunct(int c)
{
	return (isprint(c) && !isspace(c) && !isalnum(c));
}


static inline int isupper(int c)
{
	return ((c >= 'A') && (c <= 'Z'));
}

static inline int isxdigit(int c)
{
	return isdigit(c) ||
		   ((c >= 'a') && (c <= 'z')) ||
		   ((c >= 'A') && (c <= 'Z'));
}

static inline int toascii(int c)
{
	return c & 0x7F;
}

static inline int tolower(int c)
{
	const int delta = 'a' - 'A';
	return isupper(c) ? (c+delta) : c;
}

static inline int toupper(int c)
{
	const int delta = 'A' - 'a';
	return islower(c) ? (c+delta) : c;
}

#endif
