/*   
 *   copyright: Steve Dekorte, 2005. All rights reserved.
 *   license: See _BSDLicense.txt.
 *
 *   description: Bitwise methods for Array.
 */

/* --- logical ops --- */

void Array_bitwiseAnd_(Array *self, Array *other);
void Array_bitwiseOr_(Array *self, Array *other);
void Array_bitwiseXor_(Array *self, Array *other);
void Array_bitwiseCompliment(Array *self);

/* --- shifting --- */

void Array_byteShiftLeft_(Array *self, size_t n);
void Array_byteShiftRight_(Array *self, size_t n);
void Array_bitShiftLeft_(Array *self, size_t n);
void Array_bitShiftRight_(Array *self, size_t n);

/* --- get/set --- */

uint8_t Array_byteAt_(Array *self, size_t i);
uint8_t Array_bitAt_(Array *self, size_t i);
void Array_setBit_at_(Array *self, uint8_t b, size_t i);
void Array_printBits(Array *self);
unsigned int Array_bitCount(Array *self);


