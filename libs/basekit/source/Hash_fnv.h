/*
 * fnv - Fowler/Noll/Vo- hash code
 *
 * @(#) $Revision: 1.5 $
 * @(#) $Id: fnv.h,v 1.5 2003/10/03 20:35:52 chongo Exp $
 * @(#) $Source: /usr/local/src/cmd/fnv/RCS/fnv.h,v $
 *
 ***
 *
 * Fowler/Noll/Vo- hash
 *
 * The basis of this hash algorithm was taken from an idea sent
 * as reviewer comments to the IEEE POSIX P1003.2 committee by:
 *
 *      Phong Vo (http://www.research.att.com/info/kpv/)
 *      Glenn Fowler (http://www.research.att.com/~gsf/)
 *
 * In a subsequent ballot round:
 *
 *      Landon Curt Noll (http://www.isthe.com/chongo/)
 *
 * improved on their algorithm.  Some people tried this hash
 * and found that it worked rather well.  In an EMail message
 * to Landon, they named it the ``Fowler/Noll/Vo'' or FNV hash.
 *
 * FNV hashes are designed to be fast while maintaining a low
 * collision rate. The FNV speed allows one to quickly hash lots
 * of data while maintaining a reasonable collision rate.  See:
 *
 *      http://www.isthe.com/chongo/tech/comp/fnv/index.html
 *
 * for more details as well as other forms of the FNV hash.
 *
 ***
 *
 * NOTE: The FNV-0 historic hash is not recommended.  One should use
 *	 the FNV-1 hash instead.
 *
 * To use the 32 bit FNV-0 historic hash, pass FNV0_32_INIT as the
 * Fnv32_t hashval argument to fnv_32_buf() or fnv_32_str().
 *
 * To use the 64 bit FNV-0 historic hash, pass FNV0_64_INIT as the
 * Fnv64_t hashval argument to fnv_64_buf() or fnv_64_str().
 *
 * To use the recommended 32 bit FNV-1 hash, pass FNV1_32_INIT as the
 * Fnv32_t hashval argument to fnv_32_buf() or fnv_32_str().
 *
 * To use the recommended 64 bit FNV-1 hash, pass FNV1_64_INIT as the
 * Fnv64_t hashval argument to fnv_64_buf() or fnv_64_str().
 *
 * To use the recommended 32 bit FNV-1a hash, pass FNV1_32A_INIT as the
 * Fnv32_t hashval argument to fnv_32a_buf() or fnv_32a_str().
 *
 * To use the recommended 64 bit FNV-1a hash, pass FNV1_64A_INIT as the
 * Fnv64_t hashval argument to fnv_64a_buf() or fnv_64a_str().
 *
 ***
 *
 * Please do not copyright this code.  This code is in the public domain.
 *
 * LANDON CURT NOLL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL LANDON CURT NOLL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * By:
 *	chongo <Landon Curt Noll> /\oo/\
 *      http://www.isthe.com/chongo/
 *
 * Share and Enjoy!	:-)
 */

#if !defined(__FNV_H__)
#define __FNV_H__


/*
 * 32 bit FNV-0 hash type
 */
typedef unsigned long Fnv32_t;


/*
 * 32 bit FNV-0 zero initial basis
 *
 * This historic hash is not recommended.  One should use
 * the FNV-1 hash and initial basis instead.
 */
#define FNV0_32_INIT ((Fnv32_t)0)


/*
 * 32 bit FNV-1 and FNV-1a non-zero initial basis
 *
 * The FNV-1 initial basis is the FNV-0 hash of the following 32 octets:
 *
 *              chongo <Landon Curt Noll> /\../\
 *
 * NOTE: The \'s above are not back-slashing escape characters.
 * They are literal ASCII  backslash 0x5c characters.
 *
 * NOTE: The FNV-1a initial basis is the same value as FNV-1 by definition.
 */
#define FNV1_32_INIT ((Fnv32_t)0x811c9dc5)
#define FNV1_32A_INIT FNV1_32_INIT


/*
 * determine how 64 bit unsigned values are represented
 */
//#include "longlong.h"


/*
 * 64 bit FNV-0 hash
 */
#if defined(HAVE_64BIT_LONG_LONG)
typedef unsigned long long Fnv64_t;
#else
typedef struct {
    unsigned long w32[2];
} Fnv64_t;
#endif


/*
 * 64 bit FNV-0 zero initial basis
 *
 * This historic hash is not recommended.  One should use
 * the FNV-1 hash and initial basis instead.
 */
#if defined(HAVE_64BIT_LONG_LONG)
#define FNV0_64_INIT ((Fnv64_t)0)
#else
extern const Fnv64_t fnv1_64_init;
#define FNV0_64_INIT (fnv1_64_init)
#endif


/*
 * 64 bit FNV-1 non-zero initial basis
 *
 * The FNV-1 initial basis is the FNV-0 hash of the following 32 octets:
 *
 *              chongo <Landon Curt Noll> /\../\
 *
 * NOTE: The \'s above are not back-slashing escape characters.
 * They are literal ASCII  backslash 0x5c characters.
 *
 * NOTE: The FNV-1a initial basis is the same value as FNV-1 by definition.
 */
#if defined(HAVE_64BIT_LONG_LONG)
#define FNV1_64_INIT ((Fnv64_t)0xcbf29ce484222325ULL)
#define FNV1_64A_INIT FNV1_64_INIT
#else
extern const Fnv64_t fnv1_64_init;
extern const Fnv64_t fnv1_64a_init;
#define FNV1_64_INIT (fnv1_64_init)
#define FNV1_64A_INIT FNV1_64_INIT
#endif


/*
 * external functions
 */
extern Fnv32_t fnv_32_buf(void *buf, size_t len, Fnv32_t hashval);
extern Fnv32_t fnv_32_str(char *buf, Fnv32_t hashval);
extern Fnv64_t fnv_64_buf(void *buf, size_t len, Fnv64_t hashval);
extern Fnv64_t fnv_64_str(char *buf, Fnv64_t hashval);
extern Fnv32_t fnv_32a_buf(void *buf, size_t len, Fnv32_t hashval);
extern Fnv32_t fnv_32a_str(char *buf, Fnv32_t hashval);
extern Fnv64_t fnv_64a_buf(void *buf, size_t len, Fnv64_t hashval);
extern Fnv64_t fnv_64a_str(char *buf, Fnv64_t hashval);


#endif /* __FNV_H__ */