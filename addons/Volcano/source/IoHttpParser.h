#ifndef IO_Http_PARSER_DEFINED
#define IO_Http_PARSER_DEFINED 1

#include "IoObject.h"
#include "IoMessage.h"
#include "HttpParser.h"

#define MAX_HEADER_LENGTH 1024 * (80 + 32)
#define MAX_FIELD_NAME_LENGTH 256
#define MAX_FIELD_VALUE_LENGTH 80 * 1024
#define MAX_REQUEST_URI_LENGTH 1024 * 12
#define MAX_REQUEST_FRAGMENT_LENGTH 1024
#define MAX_REQUEST_PATH_LENGTH 1024
#define MAX_REQUEST_QUERY_STRING_LENGTH 1024 * 10

typedef IoObject IoHttpParser;

IoTag *IoHttpParser_newTag(void *state);
IoHttpParser *IoHttpParser_proto(void *state);
IoHttpParser *IoHttpParser_rawClone(IoHttpParser *proto);
IoHttpParser *IoHttpParser_new(void *state);
void IoHttpParser_free(IoHttpParser *self);
void IoHttpParser_initState(IoHttpParser *self);
void IoHttpParser_freeParser(IoHttpParser *self);
void IoHttpParser_initParser(IoHttpParser *self);

IoHttpParser *IoHttpParser_rawReset(IoHttpParser *self, IoObject *locals, IoMessage *m);
IoHttpParser *IoHttpParser_rawParse(IoHttpParser *self, IoObject *locals, IoMessage *m);
IoHttpParser *IoHttpParser_isFinished(IoHttpParser *self, IoObject *locals, IoMessage *m);

void IoHttpParser_setHttpField_withName_givenSize_value_givenSize_(void *data, const unsigned char *fieldNameBuffer, size_t fieldNameLength, const unsigned char *fieldValueBuffer, size_t fieldValueLength);
void IoHttpParser_setRequestURI_givenSize_(void *data, const unsigned char * buffer, size_t bufferSize);
void IoHttpParser_setFragment_givenSize_(void *data, const unsigned char * buffer, size_t bufferSize);
void IoHttpParser_setRequestPath_givenSize_(void *data, const unsigned char * buffer, size_t bufferSize);
void IoHttpParser_setQueryString_givenSize_(void *data, const unsigned char * buffer, size_t bufferSize);
void IoHttpParser_setHttpVersion_givenSize_(void *data, const unsigned char * buffer, size_t bufferSize);
void IoHttpParser_setRequestMethod_givenSize_(void *data, const unsigned char * buffer, size_t bufferSize);
void IoHttpParser_setBody_givenSize_(void *data, const unsigned char * buffer, size_t bufferSize);
#endif