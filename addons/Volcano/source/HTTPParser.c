#include "HTTPParser.h"

HTTPParser *HTTPParser_new(void)
{
	HTTPParser *self = (HTTPParser *)calloc(1, sizeof(HTTPParser));
	self->parser = (http_parser*)io_calloc(1, sizeof(http_parser));
	HTTPParser_setHTTPFieldCallback_(self, NULL);
	HTTPParser_setRequestURICallback_(self, NULL);
	HTTPParser_setFragmentCallback_(self, NULL);
	HTTPParser_setRequestPathCallback_(self, NULL);
	HTTPParser_setQueryStringCallback_(self, NULL);
	HTTPParser_setHTTPVersionCallback_(self, NULL);
	HTTPParser_setHeaderDoneCallback_(self, NULL);
	http_parser_init(self->parser);
	HTTPParser_clearParseError(self);
	return self;
}

void HTTPParser_free(HTTPParser *self)
{
	HTTPParser_clearParseError(self);
	free(self->parser);
	free(self);
}

UArray *HTTPParser_parseError(HTTPParser *self)
{
	return self->parseError;
}

void HTTPParser_setParseError_(HTTPParser *self, const char *format, ...)
{
	HTTPParser_clearParseError(self);

	va_list ap;
	va_start(ap, format);
	self->parseError = UArray_newWithVargs_(format, ap);
	va_end(ap);
	
	//move to error state
	self->parser->cs = 0;
}

void HTTPParser_clearParseError(HTTPParser *self)
{
	if(self->parseError)
	{
		UArray_free(self->parseError);
	}
	self->parseError = NULL;
}

void HTTPParser_setHTTPFieldCallback_(HTTPParser *self, field_cb callback)
{
	self->parser->http_field = callback;
}

void HTTPParser_setRequestURICallback_(HTTPParser *self, element_cb callback)
{
	self->parser->request_uri = callback;
}

void HTTPParser_setFragmentCallback_(HTTPParser *self, element_cb callback)
{
	self->parser->fragment = callback;
}

void HTTPParser_setRequestPathCallback_(HTTPParser *self, element_cb callback)
{
	self->parser->request_path = callback;
}

void HTTPParser_setQueryStringCallback_(HTTPParser *self, element_cb callback)
{
	self->parser->query_string = callback;
}

void HTTPParser_setHTTPVersionCallback_(HTTPParser *self, element_cb callback)
{
	self->parser->http_version = callback;
}

void HTTPParser_setHeaderDoneCallback_(HTTPParser *self, element_cb callback)
{
	self->parser->header_done = callback;
}

void HTTPParser_parse_fromBuffer_givenLength_(HTTPParser *self, void *listener, const char *parseBuffer, size_t parseBufferSize)
{
	HTTPParser_clearParseError(self);
	self->parser->data = listener;
	http_parser_execute(self->parser, parseBuffer, parseBufferSize, 0);
}

size_t HTTPParser_bytesParsed(HTTPParser *self)
{
	return http_parser_nread(self->parser);
}

int HTTPParser_parseErrorOccured(HTTPParser *self)
{
	return http_parser_has_error(self->parser);
}