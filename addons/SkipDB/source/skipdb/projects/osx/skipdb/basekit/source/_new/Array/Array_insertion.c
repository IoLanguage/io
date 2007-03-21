/*   
 *   copyright: Steve Dekorte, 2002. All rights reserved.
 *   license: See _BSDLicense.txt.
 */

#include "Array.h" 

void Array_insert_at_(Array *self, Array *other, size_t pos);
void Array_insertCString_at_(Array *self, char *s, size_t pos);
void Array_insertdata_length_at_(Array *self, unsigned char *data, size_t length, size_t pos);

void Array_setByteWithValue_from_to_(Array *self, unsigned char b, size_t from, size_t to);
void Array_replaceCString_withCString_(Array *self, char *s1, char *s2);


