
#include "Datum.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Datum *Datum_new(void)
{
	Datum *self = calloc(1, sizeof(Datum));
	return self;
}

Datum *Datum_newData_size_copy_(uint8_t *data, size_t size, int copy)
{
	Datum *self = Datum_new();
	
	if (copy)
	{
		Datum_setData_size_(self, data, size);
	}
	else
	{
		self->data = data;
		self->size = size;
	}
	
	return self;
}

void Datum_free(Datum *self)
{
	if (self->data) free(self->data);
	free(self);
}

void Datum_setData_size_(Datum *self, uint8_t *data, size_t size)
{
	self->data = realloc(self->data, size);
	memcpy(self->data, data, size);
}

size_t Datum_size(Datum *self)
{
	return self->size;
}

uint8_t *Datum_data(Datum *self)
{
	return self->data;
}

void Datum_show(Datum *self)
{
	printf("%s", self->data); // assume null terminated
}
