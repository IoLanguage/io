
#ifndef TagIdArray_DEFINED
#define TagIdArray_DEFINED 1

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>

typedef uint64_t tagid_t;
	
typedef struct
{
  tagid_t *ids;
  int size;
} TagIdArray;

TagIdArray *TagIdArray_new(void);
TagIdArray *TagIdArray_clone(TagIdArray *self);
void TagIdArray_copy_(TagIdArray *self, TagIdArray *other);
void TagIdArray_free(TagIdArray *self);
void TagIdArray_append_(TagIdArray *self, tagid_t v);
void TagIdArray_sort(TagIdArray *self);
void TagIdArray_removeDuplicates(TagIdArray *self);
size_t TagIdArray_size(TagIdArray *self);
tagid_t TagIdArray_at_(TagIdArray *self, size_t index);
void TagIdArray_show(TagIdArray *self);

#ifdef __cplusplus
}
#endif
#endif
