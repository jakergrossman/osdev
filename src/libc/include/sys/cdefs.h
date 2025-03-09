#ifndef _SYS_CDEFS_H
#define _SYS_CDEFS_H 1

#define __dlibc 1

/* Convenience macro to test the version of gcc.
   Use like this:
   #if __GNUC_PREREQ (2,8)
   ... code requiring gcc 2.8 or later ...
   #endif
   Note: only works for GCC 2.0 and later, because __GNUC_MINOR__ was
   added in 2.0.  */
#if defined __GNUC__ && defined __GNUC_MINOR__
# define __GNUC_PREREQ(maj, min) \
	((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
#else
# define __GNUC_PREREQ(maj, min) 0
#endif


#if (defined __has_attribute \
	 && (!defined __clang_minor__ \
		 || 3 < __clang_major__ + (5 <= __clang_minor__)))
# define __dlibc_has_attribute(attr) __has_attribute (attr)
#else
# define __dlibc_has_attribute(attr) 0
#endif
#ifdef __has_builtin
# define __dlibc_has_builtin(name) __has_builtin (name)
#else
# define __dlibc_has_builtin(name) 0
#endif
#ifdef __has_extension
# define __dlibc_has_extension(ext) __has_extension (ext)
#else
# define __dlibc_has_extension(ext) 0
#endif

/* Tell the compiler which arguments to an allocation function
   indicate the size of the allocation.  */
#if __GNUC_PREREQ (4, 3)
# define __attribute_alloc_size__(params) \
  __attribute__ ((__alloc_size__ params))
#else
# define __attribute_alloc_size__(params) /* Ignore.  */
#endif

#if __GNUC_PREREQ(2,96) || __dlibc_has_attribute(__malloc__)
# define __attribute_malloc__ __attribute__((__malloc__))
#else
# define __attribute_malloc__
#endif

#endif
