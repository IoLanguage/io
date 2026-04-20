// metadoc Common copyright Steve Dekorte 2002
// metadoc Common license BSD revised
/*metadoc Common description
This is a header that all other source files should include.
These defines are helpful for doing OS specific checks in the code.
 */

#ifndef IOCOMMON_DEFINED
#define IOCOMMON_DEFINED 1

/*#define LOW_MEMORY_SYSTEM 1*/
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include <stdint.h>

#define BASEKIT_API

/*
 [DBCS Enabling]

 DBCS (Short for Double-Byte Character Set), a character set that uses two-byte
 (16-bit) characters. Some languages, such as Chinese, Japanese and Korean
 (CJK), have writing schemes with many different characters that cannot be
 represented with single-byte codes such as ASCII and EBCDIC.

 In CJK world, CES (Character Encoding Scheme) and CCS (Coded Character Set) are
 actually different concept(one CES may contain multiple CCS). For example,
 EUC-JP is a CES which includes CCS of ASCII and JIS X 0208 (optionally JIS X
 0201 Kana and JIS X 0212).

 In Japanese (because I am Japanese),
 While EUC-JP and UTF-8 Map ASCII unchanged, ShiftJIS not (However ShiftJIS is
 de facto standard in Japan). For example, {0x95, 0x5c} represents one
 character. in ASCII, second byte(0x5c) is back slash character.
 */

/*
 check whether double-byte character. supported only ShiftJIS.
 if you want to use ShiftJIS characters in string literal, set compiler option
 -DDBCS_ENABLED=1.
 */

#if DBCS_ENABLED
#define ismbchar(c) ISSJIS((unsigned char)c)
#define mbcharlen(c) 2
#define ISSJIS(c) ((c >= 0x81 && c <= 0x9f) || (c >= 0xe0 && c <= 0xfc))
#else
#define ismbchar(c) 0
#define mbcharlen(c) 1
#endif /* DBCS_ENABLED */

#ifdef __cplusplus
extern "C" {
#endif

//#define IO_CHECK_ALLOC

#ifdef IO_CHECK_ALLOC
BASEKIT_API size_t io_memsize(void *ptr);

#define io_malloc(size) io_real_malloc(size, __FILE__, __LINE__)
BASEKIT_API void *io_real_malloc(size_t size, char *file, int line);

#define io_calloc(count, size) io_real_calloc(count, size, __FILE__, __LINE__)
BASEKIT_API void *io_real_calloc(size_t count, size_t size, char *file,
                                 int line);

#define io_realloc(ptr, size) io_real_realloc(ptr, size, __FILE__, __LINE__)
BASEKIT_API void *io_real_realloc(void *ptr, size_t newSize, char *file,
                                  int line);

BASEKIT_API void io_free(void *ptr);
BASEKIT_API void io_show_mem(char *s);
BASEKIT_API size_t io_maxAllocatedBytes(void);
BASEKIT_API void io_resetMaxAllocatedBytes(void);
BASEKIT_API size_t io_frees(void);
BASEKIT_API size_t io_allocs(void);
BASEKIT_API size_t io_allocatedBytes(void);

BASEKIT_API void io_showUnfreed(void);
#else
#define io_memsize
#define io_malloc malloc
#define io_calloc calloc
#define io_realloc io_freerealloc
#define io_free free
#define io_show_mem

#define io_maxAllocatedBytes() 0
#define io_frees() 0
#define io_allocs() 0
#define io_allocatedBytes() 0
#define io_resetMaxAllocatedBytes()
#endif

BASEKIT_API void *cpalloc(const void *p, size_t size);
BASEKIT_API void *io_freerealloc(void *p, size_t size);

int io_isBigEndian(void);
BASEKIT_API uint32_t io_uint32InBigEndian(uint32_t i);

#ifdef __cplusplus
}
#endif

#endif
