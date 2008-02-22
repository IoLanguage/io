#ifndef IO_HTTP_PARSER_DEFINED
#define IO_HTTP_PARSER_DEFINED 1

#include "IoObject.h"
#include "IoMessage.h"
#include "HTTPParser.h"

#define MAX_HEADER_LENGTH 1024 * (80 + 32)
#define MAX_FIELD_NAME_LENGTH 256
#define MAX_FIELD_VALUE_LENGTH 80 * 1024
#define MAX_REQUEST_URI_LENGTH 1024 * 12
#define MAX_REQUEST_FRAGMENT_LENGTH 1024
#define MAX_REQUEST_PATH_LENGTH 1024
#define MAX_REQUEST_QUERY_STRING_LENGTH 1024 * 10

typedef IoObject IoHTTPParser;

IoTag *IoHTTPParser_newTag(void *state);
IoHTTPParser *IoHTTPParser_proto(void *state);
IoHTTPParser *IoHTTPParser_rawClone(IoHTTPParser *proto);
IoHTTPParser *IoHTTPParser_new(void *state);
void IoHTTPParser_free(IoHTTPParser *self);

IoHTTPParser *IoHTTPParser_parse(IoHTTPParser *self, IoObject *locals, IoMessage *m);

void IoHTTPParser_setHTTPField_withName_givenSize_value_givenSize_(void *data, const unsigned char *fieldNameBuffer, size_t fieldNameLength, const unsigned char *fieldValueBuffer, size_t fieldValueLength);
void IoHTTPParser_setRequestURI_givenSize_(void *data, const unsigned char * buffer, size_t bufferSize);
void IoHTTPParser_setFragment_givenSize_(void *data, const unsigned char * buffer, size_t bufferSize);
void IoHTTPParser_setRequestPath_givenSize_(void *data, const unsigned char * buffer, size_t bufferSize);
void IoHTTPParser_setQueryString_givenSize_(void *data, const unsigned char * buffer, size_t bufferSize);
void IoHTTPParser_setHTTPVersion_givenSize_(void *data, const unsigned char * versionBuffer, size_t versionBufferSize);
#endif