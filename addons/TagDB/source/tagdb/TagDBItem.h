
#ifndef TagDBItem_DEFINED
#define TagDBItem_DEFINED 1

#ifdef __cplusplus
extern "C" {
#endif

#include "TagDBAPI.h"
#include "TagIdArray.h"
#include "KeyIdArray.h"

typedef struct
{
	keyid_t key;
	TagIdArray tags;
} TagDBItem;

TAGDB_API TagDBItem *TagDBItem_new(keyid_t key, tagid_t *tags, size_t numTags);
TAGDB_API void TagDBItem_free(TagDBItem *self);

TAGDB_API int TagDBItem_compareByKeyWith_(TagDBItem *self, TagDBItem *other);
TAGDB_API void TagDBItem_sortTags(TagDBItem *self);
TAGDB_API void TagDBItem_setTags_(TagDBItem *self, TagIdArray *tags);

TAGDB_API void TagDBItem_show(TagDBItem *self);

#ifdef __cplusplus
}
#endif
#endif
