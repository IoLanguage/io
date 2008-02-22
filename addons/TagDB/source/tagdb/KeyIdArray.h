#ifndef KeyIdArray_DEFINED
#define KeyIdArray_DEFINED 1

#ifdef __cplusplus
extern "C" {
#endif

#include "TagDBAPI.h"
#include "portable_stdint.h"
#include <stdio.h>

typedef uint64_t keyid_t;

typedef struct
{
	keyid_t *ids;
	size_t size;
} KeyIdArray;

TAGDB_API KeyIdArray *KeyIdArray_new(void);
TAGDB_API KeyIdArray *KeyIdArray_clone(KeyIdArray *self);
TAGDB_API void KeyIdArray_copy_(KeyIdArray *self, KeyIdArray *other);
TAGDB_API void KeyIdArray_free(KeyIdArray *self);
TAGDB_API keyid_t *KeyIdArray_data(KeyIdArray *self);
TAGDB_API size_t KeyIdArray_size(KeyIdArray *self);
TAGDB_API keyid_t KeyIdArray_at_(KeyIdArray *self, size_t index);
TAGDB_API void KeyIdArray_show(KeyIdArray *self);

#ifdef __cplusplus
}
#endif
#endif
