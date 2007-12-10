/*   
 *   copyright: Steve Dekorte, 2002. All rights reserved.
 *   license: See _BSDLicense.txt.
 */

#include "Array.h" 



void Array_removeLast(Array *self)
{
    if (self->size)
    {
	self->size --;
    }
}

void Array_removeAt_(Array *self, size_t index)
{
    uint8_t *v1 = Array_at_(self, index);
    uint8_t *v2 = Array_at_(self, index + 1);

    if (v1)
    {
	if (v2)
	{
	    memmove(v1, v2, self->size - index);
	}
	self->size --;
    }
}

void Array_removeFrom_to_(Array *self, size_t from, size_t to)
{
    if (to > from)
    {
	uint8_t *v1 = Array_at_(self, from);
	uint8_t *v2 = Array_at_(self, to);
	
	if (v1)
	{
	    if (v2)
	    {
		memmove(v1, v2, self->size - to);
	    }
	    
	    self->size -= to - from;
	}
    }
}


