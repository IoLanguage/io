#ifndef REQUEST_PARSER_DEFINED
#define REQUEST_PARSER_DEFINED 1

#include "http11_parser.h"
#include "UArray.h"

typedef struct
{
	http_parser *parser;
	UArray *parseError;
} HttpParser;

HttpParser *HttpParser_new(void);
void HttpParser_free(HttpParser *self);

UArray *HttpParser_parseError(HttpParser *self);
void HttpParser_setParseError_(HttpParser *self, const char *format, ...);
void HttpParser_clearParseError(HttpParser *self);
void HttpParser_setHttpFieldCallback_(HttpParser *self, field_cb callback);
void HttpParser_setRequestURICallback_(HttpParser *self, element_cb callback);
void HttpParser_setFragmentCallback_(HttpParser *self, element_cb callback);
void HttpParser_setRequestPathCallback_(HttpParser *self, element_cb callback);
void HttpParser_setQueryStringCallback_(HttpParser *self, element_cb callback);
void HttpParser_setHttpVersionCallback_(HttpParser *self, element_cb callback);
void HttpParser_setHeaderDoneCallback_(HttpParser *self, element_cb callback);
void HttpParser_setRequestMethodCallback_(HttpParser *self, element_cb callback);
void HttpParser_parse_fromBuffer_givenLength_(HttpParser *self, void *listener, const char *parseBuffer, size_t parseBufferSize);
size_t HttpParser_bytesParsed(HttpParser *self);
int HttpParser_parseErrorOccured(HttpParser *self);
int HttpParser_isFinished(HttpParser *self);

#endif