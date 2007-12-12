/*   
 *   copyright: Steve Dekorte, 2005. All rights reserved.
 *   license: See _BSDLicense.txt.
 */
 

void Array_at_putInt32_(Array *self, size_t pos, int32_t v);

 
void Array_at_putAll_(Array *self, Array *other, size_t pos);
void Array_insertData_length_at_(Array *self, unsigned char *data, size_t length, size_t pos);

// append ------------- 

void Array_appendData_length_(Array *self, unsigned char *data, size_t length);
void Array_append_(Array *self, void *s);
void Array_appendAll_(Array *self, Array *other);

// prepend ------------

void Array_prependData_length_(Array *self, unsigned char *data, size_t length);
void Array_prepend_(Array *self, void *s);
void Array_prependAll_(Array *self, Array *other);


