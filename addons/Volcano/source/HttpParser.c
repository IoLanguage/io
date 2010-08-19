#include <stdio.h>
//#include <stdarg.h>
#include "HttpParser.h"

HttpParser *HttpParser_new(void)
{
	HttpParser *self = (HttpParser *)calloc(1, sizeof(HttpParser));
	self->parser = (http_parser*)io_calloc(1, sizeof(http_parser));
	HttpParser_setHttpFieldCallback_(self, NULL);
	HttpParser_setRequestURICallback_(self, NULL);
	HttpParser_setFragmentCallback_(self, NULL);
	HttpParser_setRequestPathCallback_(self, NULL);
	HttpParser_setQueryStringCallback_(self, NULL);
	HttpParser_setHttpVersionCallback_(self, NULL);
	HttpParser_setHeaderDoneCallback_(self, NULL);
	HttpParser_setRequestMethodCallback_(self, NULL);
	http_parser_init(self->parser);
	HttpParser_clearParseError(self);
	return self;
}

void HttpParser_free(HttpParser *self)
{
	HttpParser_clearParseError(self);
	free(self->parser);
	free(self);
}

UArray *HttpParser_parseError(HttpParser *self)
{
	return self->parseError;
}

void HttpParser_setParseError_(HttpParser *self, const char *format, ...)
{
	HttpParser_clearParseError(self);

	{
		va_list ap;
		va_start(ap, format);
		self->parseError = UArray_newWithVargs_(format, ap);
		va_end(ap);
	}
	
	//move to error state
	self->parser->cs = 0;
}

void HttpParser_clearParseError(HttpParser *self)
{
	if(self->parseError)
	{
		UArray_free(self->parseError);
	}
	self->parseError = NULL;
}

void HttpParser_setHttpFieldCallback_(HttpParser *self, field_cb callback)
{
	self->parser->http_field = callback;
}

void HttpParser_setRequestURICallback_(HttpParser *self, element_cb callback)
{
	self->parser->request_uri = callback;
}

void HttpParser_setFragmentCallback_(HttpParser *self, element_cb callback)
{
	self->parser->fragment = callback;
}

void HttpParser_setRequestPathCallback_(HttpParser *self, element_cb callback)
{
	self->parser->request_path = callback;
}

void HttpParser_setQueryStringCallback_(HttpParser *self, element_cb callback)
{
	self->parser->query_string = callback;
}

void HttpParser_setHttpVersionCallback_(HttpParser *self, element_cb callback)
{
	self->parser->http_version = callback;
}

void HttpParser_setHeaderDoneCallback_(HttpParser *self, element_cb callback)
{
	self->parser->header_done = callback;
}

void HttpParser_setRequestMethodCallback_(HttpParser *self, element_cb callback)
{
	self->parser->request_method = callback;
}

void HttpParser_parse_fromBuffer_givenLength_(HttpParser *self, void *listener, const char *parseBuffer, size_t parseBufferSize)
{
	HttpParser_clearParseError(self);
	self->parser->data = listener;
	http_parser_execute(self->parser, parseBuffer, parseBufferSize, HttpParser_bytesParsed(self));
}

size_t HttpParser_bytesParsed(HttpParser *self)
{
	return http_parser_nread(self->parser);
}

int HttpParser_parseErrorOccured(HttpParser *self)
{
	return http_parser_has_error(self->parser);
}

int HttpParser_isFinished(HttpParser *self)
{
	return http_parser_is_finished(self->parser);
}
