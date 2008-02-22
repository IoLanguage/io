#ifndef REQUEST_PARSER_DEFINED
#define REQUEST_PARSER_DEFINED 1

#include "http11_parser.h"
#include "UArray.h"

typedef struct
{
	http_parser *parser;
	UArray *parseError;
} HTTPParser;

HTTPParser *HTTPParser_new(void);
void HTTPParser_free(HTTPParser *self);

UArray *HTTPParser_parseError(HTTPParser *self);
void HTTPParser_setParseError_(HTTPParser *self, const char *format, ...);
void HTTPParser_clearParseError(HTTPParser *self);
void HTTPParser_setHTTPFieldCallback_(HTTPParser *self, field_cb callback);
void HTTPParser_setRequestURICallback_(HTTPParser *self, element_cb callback);
void HTTPParser_setFragmentCallback_(HTTPParser *self, element_cb callback);
void HTTPParser_setRequestPathCallback_(HTTPParser *self, element_cb callback);
void HTTPParser_setQueryStringCallback_(HTTPParser *self, element_cb callback);
void HTTPParser_setHTTPVersionCallback_(HTTPParser *self, element_cb callback);
void HTTPParser_setHeaderDoneCallback_(HTTPParser *self, element_cb callback);
void HTTPParser_parse_fromBuffer_givenLength_(HTTPParser *self, void *listener, const char *parseBuffer, size_t parseBufferSize);
size_t HTTPParser_bytesParsed(HTTPParser *self);
int HTTPParser_parseErrorOccured(HTTPParser *self);

#endif