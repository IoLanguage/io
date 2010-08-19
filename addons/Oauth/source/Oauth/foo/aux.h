#ifndef IO_AUX_H
#define IO_AUX_H
#include <stdint.h>
#include <string.h>

struct string
{
	size_t size;
	size_t len;
	char *data;
};

struct vector
{
	size_t size;
	size_t len;
	size_t item_size;
	void *values;
};

struct allocator
{
	size_t bankCapacity;
	struct vector banks;
	size_t curBankUtilization;
	char *curBank;
	uint32_t curBanksIndex;
};



#define vector_value(v, index)  (((char *)(v)->values) + ((index) * (v)->item_size))
#define vector_values(v)  	(v)->values
#define vector_size(v) 		(v)->len

void *vector_pop(struct vector *self);
void *vector_push(struct vector *self, const void *p);
void *vector_pushnew(struct vector *self);
void vector_dealloc(struct vector *self);
void vector_init(struct vector *self, const size_t is);
void vector_set_capacity(struct vector *self, const size_t n);
void vector_reset(struct vector *self);



#define string_len(s) (s)->len
#define string_data(s) (s)->data


size_t string_capacity(struct string *self);
void string_init(struct string *self);
void string_dealloc(struct string *self);
void string_set_capacity(struct string *self, const size_t n);
void string_append(struct string *self, const void *p, const size_t l);
void string_appendfmt(struct string *self, const char *fmt, ...);
void string_reset(struct string *);
void string_append_urlencoded_rfc3986(struct string *, const char *, const size_t);
void string_adjustlen(struct string *, const int factor);



void allocator_init(struct allocator *self, const size_t bc);
void allocator_dealloc(struct allocator *self);
void *allocator_alloc(struct allocator *self, const size_t size);
void allocator_reuse(struct allocator *self);

static inline char ToHex(const int v)
{
	if (v < 10)
		return '0' + v;
	return 'A' + (v - 10);
}

static inline int FromHex(const char v)
{
	if (v >='0' && v<='9')
		return v - '0';
	else if (v >='a' && v <='f')
		return v - 'a' + 10;
	else if (v >='A' && v <='F')
		return v - 'A' + 10;
}

void base64_encode(uint8_t *data, size_t length, struct string *outputBuf, const int forHeaders);
#endif
