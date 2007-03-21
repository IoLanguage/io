/*   
 *   copyright: Steve Dekorte, 2002. All rights reserved.
 *   license: See _BSDLicense.txt.
 */

#include "Array_bitwise.h" 

/* --- bitwise ops ------------------------------------------------ */

void Array_and_(Array *self, Array *other)
{
    int l1 = Array_size(self);
    int l2 = Array_size(other);
    int i = l1 < l2 ? l1 : l2;
    unsigned char *b1 = Array_bytes(self);
    unsigned char *b2 = Array_bytes(other);
    
    while (i)
    {
        *b1 = *b1 & *b2;
	b1 ++; b2 ++;
	i --;
    }
}

void Array_or_(Array *self, Array *other)
{
    int l1 = Array_size(self);
    int l2 = Array_size(other);
    int i = l1 < l2 ? l1 : l2;
    unsigned char *b1 = Array_bytes(self);
    unsigned char *b2 = Array_bytes(other);
    
    while (i)
    {
        *b1 = *b1 | *b2;
	b1 ++; b2 ++;
	i --;
    }
}

void Array_xor_(Array *self, Array *other)
{
    int l1 = Array_size(self);
    int l2 = Array_size(other);
    int i = l1 < l2 ? l1 : l2;
    unsigned char *b1 = Array_bytes(self);
    unsigned char *b2 = Array_bytes(other);
    
    while (i)
    {
        *b1 = *b1 ^ *b2;
	b1 ++; b2 ++;
	i --;
    }
}

void Array_compliment(Array *self)
{
    int i = Array_size(self);
    unsigned char *b = Array_bytes(self);
    
    while (i)
    {
        *b = ~ (*b);
	b ++; 
	i --;
    }
}

void Array_byteShiftLeft_(Array *self, int s)
{
    Array *ba = Array_new();
    Array_setLength_(ba, s);
    Array_insert_at_(self, ba, 0);
}

void Array_byteShiftRight_(Array *self, int s)
{
    Array *ba = Array_new();
    Array_setLength_(ba, s);
    Array_removeFrom_to_(self, 0, s);
    Array_append_(self, ba);
}

void Array_bitShiftLeft_(Array *self, int s)
{
    int bytes = s / 8;
    int bits  = s % 8;
    int carryBits  = 8 - bits;
    
    if (bytes > 0) 
    { 
	Array_byteShiftLeft_(self, bytes); 
    }
    else 
    { 
	Array_byteShiftRight_(self, bytes); 
    }
    
    {
	int i = Array_size(self);
	unsigned char *b = Array_bytes(self);
	unsigned char carry = 0x0;
	
	while (i)
	{
	    *b = *b << s;
	    *b = *b | carry;
	    carry = *b >> carryBits;
	    b ++; 
	    i --;
	}
    }
}

unsigned char Array_byteAt_(Array *self, size_t i)
{
    if (i < self->length) 
    {
	return self->bytes[i];
    }
    
    return 0x0;  
}

int Array_bitAt_(Array *self, size_t i)
{
    size_t byteIndex = i / 8;
    int bitIndex = i % 8;
    unsigned char byte = Array_byteAt_(self, byteIndex);
    
    return ((byte >> bitIndex) & 0x1);  
}

void Array_setBit_at_(Array *self, int state, size_t i)
{
    size_t charPos = i / 8;
    
    if (state) 
    { 
	state = 0x1; 
    } 
    else 
    { 
	state = 0x0; 
    }
    
    if (charPos < self->length)
    {
	unsigned char bitPos = i % 8;
	unsigned char mask = 0x1 << bitPos;
	
	if (state)
	{
	    self->bytes[charPos] |= mask;
	}
	else
	{
	    self->bytes[charPos] &= (~mask);
	}
    }
    
    if (Array_bitAt_(self, i) != state)
    {
	printf("error\n");
	Array_bitAt_(self, i);
    }
}
 
 