
#ifndef Datum_DEFINED
#define Datum_DEFINED 1

#ifdef __cplusplus
extern "C" {
#endif

#include "TagDBAPI.h"
#include "portable_stdint.h"
#include <stdio.h>
#include "TagDBAPI.h"

typedef struct
{
  uint8_t *data;
  size_t size;
} Datum;

TAGDB_API Datum *Datum_new(void);
TAGDB_API Datum *Datum_newData_size_copy_(uint8_t *data, size_t size, int copy);
TAGDB_API void Datum_free(Datum *self);
TAGDB_API void Datum_setData_size_(Datum *self, uint8_t *data, size_t size);
TAGDB_API size_t Datum_size(Datum *self);
TAGDB_API uint8_t *Datum_data(Datum *self);
TAGDB_API void Datum_show(Datum *self);

#ifdef __cplusplus
}
#endif
#endif
