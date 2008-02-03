
#include <depot.h>
#include <cabin.h>
#include <villa.h>

#ifndef villa_extras_DEFINED
#define villa_extras_DEFINED 1

#ifdef __cplusplus
extern "C" {
#endif

VILLA *vlopen_andRepairIfNeeded(char *path, int options, VLCFUNC func);
int VL_CMPUINT(const char *a, int aSize, const char *b, int bSize);

#ifdef __cplusplus
}
#endif
#endif
