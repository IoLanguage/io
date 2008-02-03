
#include "TagIdArray.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

TagIdArray *TagIdArray_new(void)
{
	TagIdArray *self = calloc(1, sizeof(TagIdArray));
	return self;
}

void TagIdArray_append_(TagIdArray *self, tagid_t v)
{
	self->ids = realloc(self->ids, (self->size + 1) * sizeof(tagid_t));
	self->ids[self->size] = v;
	self->size ++;
}

TagIdArray *TagIdArray_clone(TagIdArray *self)
{
	TagIdArray *ka = TagIdArray_new();
	TagIdArray_copy_(ka, self);
	return ka;
}

void TagIdArray_copy_(TagIdArray *self, TagIdArray *other)
{
	self->size = other->size;
	self->ids = realloc(self->ids, other->size * sizeof(tagid_t));
	memcpy(self->ids, other->ids, other->size * sizeof(tagid_t));
}

static int TagIdCompare(const void *a, const void *b)
{
	const tagid_t aa = *(tagid_t *)a;
	const tagid_t bb = *(tagid_t *)b;
	if (aa == bb) return 0;
	return (aa > bb) ? 1 : -1;
}

void TagIdArray_sort(TagIdArray *self)
{
	qsort(self->ids, self->size, sizeof(tagid_t), TagIdCompare);
}

void TagIdArray_removeDuplicates(TagIdArray *self)
{
	//qsort(self->ids, self->size, sizeof(tagid_t), TagIdCompare);
}

size_t TagIdArray_size(TagIdArray *self)
{
	return self->size;
}

tagid_t TagIdArray_at_(TagIdArray *self, size_t index)
{
	return self->ids[index];
}

void TagIdArray_show(TagIdArray *self)
{
	size_t i;
	
	printf("[");
	
	for (i = 0; i < self->size; i ++)
	{
		if (i > 0) printf(" ");
		printf("%i", (int)self->ids[i]);
	}
	
	printf("]");
}

void TagIdArray_free(TagIdArray *self)
{
	if (self->ids) free(self->ids);
	free(self);
}

