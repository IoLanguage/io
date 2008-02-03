#ifndef KeyIdArray_DEFINED
#define KeyIdArray_DEFINED 1

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>

typedef uint64_t keyid_t;

typedef struct
{
	keyid_t *ids;
	size_t size;
} KeyIdArray;

KeyIdArray *KeyIdArray_new(void);
KeyIdArray *KeyIdArray_clone(KeyIdArray *self);
void KeyIdArray_copy_(KeyIdArray *self, KeyIdArray *other);
void KeyIdArray_free(KeyIdArray *self);
keyid_t *KeyIdArray_data(KeyIdArray *self);
size_t KeyIdArray_size(KeyIdArray *self);
keyid_t KeyIdArray_at_(KeyIdArray *self, size_t index);
void KeyIdArray_show(KeyIdArray *self);

#ifdef __cplusplus
}
#endif
#endif
