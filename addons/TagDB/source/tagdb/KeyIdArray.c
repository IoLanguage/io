#include "KeyIdArray.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

KeyIdArray *KeyIdArray_new(void)
{
	KeyIdArray *self = calloc(1, sizeof(KeyIdArray));
	return self;
}

KeyIdArray *KeyIdArray_clone(KeyIdArray *self)
{
	KeyIdArray *ka = KeyIdArray_new();
	KeyIdArray_copy_(ka, self);
	return ka;
}

void KeyIdArray_copy_(KeyIdArray *self, KeyIdArray *other)
{
	self->size = other->size;
	self->ids = realloc(self->ids, other->size * sizeof(keyid_t));
	memcpy(self->ids, other->ids, other->size * sizeof(keyid_t));
}

void KeyIdArray_free(KeyIdArray *self)
{
	if (self->ids) free(self->ids);
	free(self);
}

keyid_t *KeyIdArray_data(KeyIdArray *self)
{
	return self->ids;
}

size_t KeyIdArray_size(KeyIdArray *self)
{
	return self->size;
}

keyid_t KeyIdArray_at_(KeyIdArray *self, size_t index)
{
	return self->ids[index];
}

void KeyIdArray_show(KeyIdArray *self)
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

