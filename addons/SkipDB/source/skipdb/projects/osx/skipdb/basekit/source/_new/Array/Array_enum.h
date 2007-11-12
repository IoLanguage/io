/*   
 *   copyright: Steve Dekorte, 2002. All rights reserved.
 *   license: See _BSDLicense.txt.
 *
 *   description: Enumeration methods for Array.
 */
 

#ifndef Array_enum_DEFINED
#define Array_enum_DEFINED 1

#include "Array.h"

/* --- enumeration --------------------------------------------*/

typedef void (ArrayDoFunc)(void *);
void Array_do_(Array *self, ArrayDoFunc *func);

typedef void (ArrayDoWithFunc)(void *, void *);
void Array_do_with_(Array *self, ArrayDoWithFunc *func, void *arg);

typedef void (ArrayTargetDoFunc)(void *, void *);
void Array_target_do_(Array *self, void *target, ArrayTargetDoFunc *func);

typedef void (ArrayDetectFunc)(void *);
int Array_detect_(Array *self, (ArrayDetectFunc *)func);

/*
typedef void (ArrayDetectFunc)(void *);
Array *Array_collect_withTarget_(Array *self, ArrayForeach *func, void *target);
*/

#ifdef __cplusplus
}
#endif
#endif

