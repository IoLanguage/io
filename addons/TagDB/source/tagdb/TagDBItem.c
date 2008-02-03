#include "TagDB.h"
#include <stdlib.h>
#include <string.h>

TagDBItem *TagDBItem_new(keyid_t key, tagid_t *tags, size_t numTags)
{
	TagDBItem *self = calloc(1, sizeof(TagDBItem));
	
	self->key = key;
	self->tags.ids = malloc(numTags * sizeof(tagid_t));
	memcpy(self->tags.ids, tags, numTags * sizeof(tagid_t));
	self->tags.size = numTags;
	
	TagIdArray_sort(&(self->tags));
	TagIdArray_removeDuplicates(&(self->tags));

	return self;
}

void TagDBItem_free(TagDBItem *self)
{
	free(self->tags.ids);
	free(self);
}

int TagDBItem_compareByKeyWith_(TagDBItem *self, TagDBItem *other)
{
	if (self->key == other->key) return 0;
	return (self->key > other->key) ? 1 : -1;
}

void TagDBItem_sortTags(TagDBItem *self)
{
	TagIdArray_sort(&(self->tags));
}

void TagDBItem_setTags_(TagDBItem *self, TagIdArray *tags)
{
	self->tags.ids = realloc(self->tags.ids, tags->size);
	memmove(self->tags.ids, tags->ids, tags->size * sizeof(tagid_t));
	TagDBItem_sortTags(self);
	TagIdArray_removeDuplicates(tags);
}

void TagDBItem_show(TagDBItem *self)
{
	int i;
	
	printf("%i [", (int)self->key);
	
	for (i = 0; i < self->tags.size; i ++)
	{
		if (i > 0) printf(" ");
		printf("%i", (int)self->tags.ids[i]);
	}
	printf("]\n");
}
