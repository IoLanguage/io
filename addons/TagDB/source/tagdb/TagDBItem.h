
#ifndef TagDBItem_DEFINED
#define TagDBItem_DEFINED 1

#ifdef __cplusplus
extern "C" {
#endif

#include "TagIdArray.h"
#include "KeyIdArray.h"

typedef struct
{
	keyid_t key;
	TagIdArray tags;
} TagDBItem;

TagDBItem *TagDBItem_new(keyid_t key, tagid_t *tags, size_t numTags);
void TagDBItem_free(TagDBItem *self);

int TagDBItem_compareByKeyWith_(TagDBItem *self, TagDBItem *other);
void TagDBItem_sortTags(TagDBItem *self);
void TagDBItem_setTags_(TagDBItem *self, TagIdArray *tags);

void TagDBItem_show(TagDBItem *self);

#ifdef __cplusplus
}
#endif
#endif
