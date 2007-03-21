/*#io
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
*/
 
#include "Common.h"

void *cpalloc(void *p, size_t size)
{
    void *n = malloc(size);
    memcpy(n, p, size);
    return n;
}

