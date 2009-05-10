#ifndef PORTABLESORTING_DEFINED
#define PORTABLESORTING_DEFINED 1

#include "Common.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
   The reason for using this instead of C's qsort is
   that we need more context information than just the
   two objects if we want to do something like use an
   Io block to do the comparison and using globals is
   unacceptable for several reasons.
   
   qsort_r isn't available on all platforms.
*/

//typedef int   (ListSortRCallback)(void *, const void *, const void *);

typedef int (*PortableSortingCompareCallback)(void *context, const void *a, const void *b);

BASEKIT_API void portable_qsort_r(void *base, size_t nel, size_t width, 
	void *context, PortableSortingCompareCallback compare);

#ifdef __cplusplus
}
#endif

#endif
