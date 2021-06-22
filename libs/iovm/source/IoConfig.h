#define _POSIX_C_SOURCE
#define IO_COMPILER_TYPE gcc

/* ---------------------------------------------------------- */

#include <setjmp.h>

/* Solaris 9 Sparc with GCC */
#if defined(__SVR4) && defined(__sun)

#define IO_OS_TYPE solaris
#define IO_COMPILER_TYPE gcc

#if defined(_JBLEN) && (_JBLEN == 12) && defined(__sparc)
#define IO_PROCESSOR_TYPE powerpc

/* Solaris 9 i386 with GCC */
#elif defined(_JBLEN) && (_JBLEN == 10) && defined(__i386)
#define IO_PROCESSOR_TYPE i386

#endif
#endif

/* #if defined(__MACOSX__) && defined(_BSD_PPC_SETJMP_H_) */
#if defined(_BSD_PPC_SETJMP_H_)
/* OSX/PPC */
#define IO_OS_TYPE darwin
#define IO_PROCESSOR_TYPE powerpc

#elif defined(sgi) && defined(_IRIX4_SIGJBLEN)
/* Irix/SGI */
#define IO_OS_TYPE irix
#define IO_PROCESSOR_TYPE mips

#elif defined(linux)
/* Various flavors of Linux. */
#if defined(JB_GPR1)
/* Linux/PPC */

#define IO_OS_TYPE linux
#define IO_PROCESSOR_TYPE powerpc

#elif defined(JB_RBX)
/* Linux/Opteron */

#define IO_OS_TYPE linux
#define IO_PROCESSOR_TYPE i386

#elif defined(JB_SP)
/* Linux/i386 with glibc2 */

#define IO_OS_TYPE linux
#define IO_PROCESSOR_TYPE i386
#define IO_LIBC_TYPE glibc2

#elif defined(_I386_JMP_BUF_H)
/* i386-linux with libc5 */
#define IO_OS_TYPE linux
#define IO_PROCESSOR_TYPE i386
#define IO_LIBC_TYPE libc5

#elif defined(__JMP_BUF_SP)
/* arm-linux on the sharp zauras */
#define IO_OS_TYPE linux
#define IO_PROCESSOR_TYPE arm

#endif

#elif defined(__CYGWIN__)
#define IO_OS_TYPE cygwin

#elif defined(__MINGW32__)
#define IO_OS_TYPE mingw

#elif defined(_MSC_VER)
/* win32 visual c - should this be the same as __MINGW32__? */
#define IO_OS_TYPE win32

#elif defined(__SYMBIAN32__)
#define IO_OS_TYPE symbian

#elif defined(__FreeBSD__)
#define IO_OS_TYPE io_freebsd

#if defined(_JBLEN) && (_JBLEN == 81)
/* FreeBSD/Alpha */
#define IO_PROCESSOR_TYPE alpha

#elif defined(_JBLEN)
/* FreeBSD on IA32 */
#define IO_PROCESSOR_TYPE i386
#endif

#elif defined(__NetBSD__)
/* NetBSD. */
/* NetBSD i386. */
#define IO_OS_TYPE netbsd
#define IO_PROCESSOR_TYPE i386
#endif

#elif defined(__SVR4) && defined(__sun)
/* Solaris. */
#if defined(SUN_PROGRAM_COUNTER)
/* SunOS 9 */
#define IO_OS_TYPE sunos
#endif
#endif
