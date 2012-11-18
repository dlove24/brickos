/**
*** Copyright (c) 2011 David Love <david@homeunix.org.uk>
***
*** Permission to use, copy, modify, and/or distribute this software for any
*** purpose with or without fee is hereby granted, provided that the above
*** copyright notice and this permission notice appear in all copies.
***
*** THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
*** WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
*** MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
*** ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
*** WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
*** ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
*** OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
***
*** \file config.h
*** \brief Provides defines for platform, compiler and header options from CMake
***
*** \author David Love
*** \date December 2011
**/

/**
*** Platform Checks
**/

/* Look for the machine type on Apple platforms at compile (not configure). Apple platforms
** allow multiple architectures to exit simultaneously, so we can't check at configure
**
** See: http://www.cmake.org/pipermail/cmake/2009-June/030126.html
**/

#if defined(__APPLE__)
#  if defined(__i386__)
#    undef HAVE_64_BIT
#    undef HAVE_BIG_ENDIAN
#  elif defined(__ppc__)
#    undef HAVE_64_BIT
#    define HAVE_BIG_ENDIAN
#  elif defined(__x86_64__)
#    define HAVE_64_BIT
#    undef HAVE_BIG_ENDIAN
#  elif defined(__ppc64__)
#    define HAVE_64_BIT
#    define HAVE_BIG_ENDIAN
#  else
      // oops
#    error "Unknown architecture!"
#  endif
#else
/* #undef HAVE_64_BIT */
/* #undef HAVE_BIG_ENDIAN */
#endif

/**
*** Function Declarations
**/

/* Look for the vsnprintf function, used by the bstring library */
/* #undef BSTRLIB_VSNP_OK */

/* Look for functions from the C Standard Library */
/* #undef HAVE_MALLOC */
/* #undef HAVE_REALLOC */

/* Look for various platform defines. This are usually present, but may
 * be missing (especially on MS Windows)
 */

/* #undef HAVE_FILENO */
/* #undef HAVE_GETOPT */
/* #undef HAVE_LOCALTIME_R */
/* #undef HAVE_PIPE */
/* #undef HAVE_PUTENV */
/* #undef HAVE_SETENV */
/* #undef HAVE_SETENV */
/* #undef HAVE_SLEEP */
/* #undef HAVE_STRDUP */
/* #undef HAVE_STRSIGNAL */
/* #undef HAVE_UNSETENV */

/**
*** Header Declarations
**/

/* Look for assertions */
/* #undef HAVE_ASSERT_H */

/* Look for the C99 declaration of the standard types */
/* #undef HAVE_STDINT_H */

/* Look for the C99 integer types: needed for the bit matrix amongst other things */
/* #undef HAVE_INTTYPES_H */

/* Look for the header files defining the constants for type lengths */
/* #undef HAVE_FLOAT_H */
/* #undef HAVE_LIMITS_H */
/* #undef HAVE_VALUES_H */

/* Look for the standard library files, needed for access to the C library */
/* #undef HAVE_CTYPE_H */
/* #undef HAVE_STDLIB_H */
/* #undef HAVE_STDIO_H */

/* Look for the standard string library, used for handling C style strings 
 * NOTE: Should only be used for legacy, external code. New code should
 * prefer the BString library
 */
/* #undef HAVE_CTYPE_H */
/* #undef HAVE_STRING_H */
/* #undef HAVE_STRINGS_H */

/* Look for the getopt library. If this is not available, routines
 * should use the provided implementation of the ArgTable library.
 * Even better, just use the ArgTable library...
 */
/* #undef HAVE_GETOPT_H */

/* Look for the C routines for formatted output conversions */
/* #undef HAVE_STDARG_H */

/* Look for various POSIX functions */
/* #undef HAVE_UNISTD_H */
/* #undef HAVE_SYS_WAIT_H */

/* Look for the POSIX thread library */
/* #undef HAVE_PTHREAD_H */

/**
*** Library Constants
**/


