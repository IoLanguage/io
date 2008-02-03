
#ifndef Datum_DEFINED
#define Datum_DEFINED 1

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
	
typedef struct
{
  uint8_t *data;
  size_t size;
} Datum;

Datum *Datum_new(void);
Datum *Datum_newData_size_copy_(uint8_t *data, size_t size, int copy);
void Datum_free(Datum *self);
void Datum_setData_size_(Datum *self, uint8_t *data, size_t size);
size_t Datum_size(Datum *self);
uint8_t *Datum_data(Datum *self);
void Datum_show(Datum *self);

#ifdef __cplusplus
}
#endif
#endif
