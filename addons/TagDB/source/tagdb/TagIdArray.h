
#ifndef TagIdArray_DEFINED
#define TagIdArray_DEFINED 1

#ifdef __cplusplus
extern "C" {
#endif

#include "TagDBAPI.h"
#include "portable_stdint.h"
#include <stdio.h>

typedef uint64_t tagid_t;

typedef struct
{
  tagid_t *ids;
  int size;
} TagIdArray;

TAGDB_API TagIdArray *TagIdArray_new(void);
TAGDB_API TagIdArray *TagIdArray_clone(TagIdArray *self);
TAGDB_API void TagIdArray_copy_(TagIdArray *self, TagIdArray *other);
TAGDB_API void TagIdArray_free(TagIdArray *self);
TAGDB_API void TagIdArray_append_(TagIdArray *self, tagid_t v);
TAGDB_API void TagIdArray_sort(TagIdArray *self);
TAGDB_API void TagIdArray_removeDuplicates(TagIdArray *self);
TAGDB_API size_t TagIdArray_size(TagIdArray *self);
TAGDB_API tagid_t TagIdArray_at_(TagIdArray *self, size_t index);
TAGDB_API void TagIdArray_show(TagIdArray *self);

#ifdef __cplusplus
}
#endif
#endif
