#include "IoHTTPParser.h"
#include "IoMap.h"
#include "IoState.h"
#include "http11_parser.h"

#define PARSER(self) (HTTPParser *)IoObject_dataPointer(self)

IoTag *IoHTTPParser_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("HTTPParser");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoHTTPParser_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoHTTPParser_rawClone);
	return tag;
}

IoHTTPParser *IoHTTPParser_proto(void *state)
{
	IoObject *self = IoObject_new(state);

	IoObject_tag_(self, IoHTTPParser_newTag(state));

	IoState_registerProtoWithFunc_(state, self, IoHTTPParser_proto);
	
	IoHTTPParser_initState(self);

	{
		IoMethodTable methodTable[] = {
		{"parse", IoHTTPParser_parse},
		{"isFinished", IoHTTPParser_isFinished},
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoHTTPParser *IoHTTPParser_rawClone(IoHTTPParser *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);

	IoHTTPParser_initState(self);

	return self;
}

IoHTTPParser *IoHTTPParser_new(void *state)
{
	IoHTTPParser *proto = IoState_protoWithInitFunction_(state, IoHTTPParser_proto);
	return IOCLONE(proto);
}

void IoHTTPParser_free(IoHTTPParser *self)
{
	HTTPParser_free(PARSER(self));
}

void IoHTTPParser_initState(IoHTTPParser *self)
{
	HTTPParser *parser = HTTPParser_new();

	HTTPParser_setHTTPFieldCallback_(parser, (field_cb) IoHTTPParser_setHTTPField_withName_givenSize_value_givenSize_);
	HTTPParser_setRequestURICallback_(parser, (element_cb) IoHTTPParser_setRequestURI_givenSize_);
	HTTPParser_setFragmentCallback_(parser, (element_cb) IoHTTPParser_setFragment_givenSize_);
	HTTPParser_setRequestPathCallback_(parser, (element_cb) IoHTTPParser_setRequestPath_givenSize_);
	HTTPParser_setQueryStringCallback_(parser, (element_cb) IoHTTPParser_setQueryString_givenSize_);
	HTTPParser_setHTTPVersionCallback_(parser, (element_cb) IoHTTPParser_setHTTPVersion_givenSize_);
	HTTPParser_setHeaderDoneCallback_(parser, (element_cb) IoHTTPParser_setBody_givenSize_);
	IoObject_setDataPointer_(self, parser);

	IoObject_setSlot_to_(self, IOSYMBOL("httpFields"), IoMap_new(IOSTATE));
}

/* ----------------------------------------------------------- */

IoHTTPParser *IoHTTPParser_parse(IoHTTPParser *self, IoObject *locals, IoMessage *m)
{
	//doc HTTPParser parse Parsers the HTTP request stored in the parseBuffer slot.  Sets slots on self using the values parsed.  Returns self if successful or an Error if the parse fails.
	HTTPParser *parser = PARSER(self);
	IoSeq *parseBuffer = IoObject_getSlot_(self, IOSYMBOL("parseBuffer"));
	IoMap *httpFields = IoObject_getSlot_(self, IOSYMBOL("httpFields"));
	
	IOASSERT(ISSEQ(parseBuffer), "The parseBuffer should be set to a Sequence before parse is called.");
	IOASSERT(IoSeq_rawSize(parseBuffer) > 0, "The parseBuffer can't be empty when parse is called.");
	
	IOASSERT(ISMAP(httpFields), "httpFields should be set to a Map before parse is called.");

	HTTPParser_parse_fromBuffer_givenLength_(parser, self, IoSeq_asCString(parseBuffer), IoSeq_rawSizeInBytes(parseBuffer));
	
	if (HTTPParser_bytesParsed(parser) > MAX_HEADER_LENGTH)
	{
		return IoState_setErrorDescription_(IOSTATE, "Header is longer than the allowed length of %d bytes.", MAX_HEADER_LENGTH);
	}

	if (HTTPParser_parseErrorOccured(parser))
	{
		UArray *parseError = HTTPParser_parseError(parser);
		if (parseError)
		{
			return IoState_setErrorDescription_(IOSTATE, UArray_asCString(parseError));
		}
		else
		{
			return IoState_setErrorDescription_(IOSTATE, "Invalid HTTP format.  Parse failed.");
		}
	}

	return self;
}

IoHTTPParser *IoHTTPParser_isFinished(IoHTTPParser *self, IoObject *locals, IoMessage *m)
{
	//doc HTTPParser isFinished Returns true if the parser has fully completed parsing of the content in the parse buffer and false if it hasn't.
	HTTPParser *parser = PARSER(self);

	return IOBOOL(self, HTTPParser_isFinished(parser));
}
/* ----------------------------------------------------------- */

void IoHTTPParser_setHTTPField_withName_givenSize_value_givenSize_(void *data, const unsigned char *fieldNameBuffer, size_t fieldNameSize, const unsigned char *fieldValueBuffer, size_t fieldValueSize)
{
	IoHTTPParser *self = (IoHTTPParser *)data;
	
	if (fieldNameSize > MAX_FIELD_NAME_LENGTH)
	{
		HTTPParser *parser = PARSER(self);
		HTTPParser_setParseError_(parser, "HTTP field name is longer than the allowed length of %d bytes.", MAX_FIELD_NAME_LENGTH);
		return;
	}

	if (fieldValueSize > MAX_FIELD_VALUE_LENGTH)
	{
		HTTPParser *parser = PARSER(self);
		HTTPParser_setParseError_(parser, "HTTP field value is longer than the allowed length of %d bytes.", MAX_FIELD_VALUE_LENGTH);
		return;
	}

	{
		IoMap *httpFields = IoObject_getSlot_(self, IOSYMBOL("httpFields"));
		UArray *fieldName = UArray_newWithData_type_size_copy_((char *)fieldNameBuffer, CTYPE_uint8_t, fieldNameSize, 1);
		IoSeq *fieldValue = IOSEQ(fieldValueBuffer, fieldValueSize);

		//UArray_toupper(fieldName);
		//UArray_replaceCString_withCString_(fieldName, "-", "_");

		//IoObject_setSlot_to_(self, IoState_symbolWithUArray_copy_((IoState*)IOSTATE, fieldName, 0), fieldValue);
		IoMap_rawAtPut(httpFields, IoState_symbolWithUArray_copy_((IoState*)IOSTATE, fieldName, 0), fieldValue);
	}
}

void IoHTTPParser_setRequestURI_givenSize_(void *data, const unsigned char * buffer, size_t bufferSize)
{
	IoHTTPParser *self = (IoHTTPParser *)data;
	
	if(bufferSize > MAX_REQUEST_URI_LENGTH)
	{
		HTTPParser *parser = PARSER(self);
		HTTPParser_setParseError_(parser, "requestURI is longer than the allowed length of %d bytes.", MAX_REQUEST_URI_LENGTH);
		return;
	}
	
	IoObject_setSlot_to_(self, IOSYMBOL("requestURI"), IOSEQ(buffer, bufferSize));
}

void IoHTTPParser_setFragment_givenSize_(void *data, const unsigned char * buffer, size_t bufferSize)
{
	IoHTTPParser *self = (IoHTTPParser *)data;
	
	if(bufferSize > MAX_REQUEST_FRAGMENT_LENGTH)
	{
		HTTPParser *parser = PARSER(self);
		HTTPParser_setParseError_(parser, "fragment is longer than the allowed length of %d bytes.", MAX_REQUEST_FRAGMENT_LENGTH);
		return;
	}
	IoObject_setSlot_to_(self, IOSYMBOL("fragment"), IOSEQ(buffer, bufferSize));
}

void IoHTTPParser_setRequestPath_givenSize_(void *data, const unsigned char * buffer, size_t bufferSize)
{
	IoHTTPParser *self = (IoHTTPParser *)data;
	
	if(bufferSize > MAX_REQUEST_PATH_LENGTH)
	{
		HTTPParser *parser = PARSER(self);
		HTTPParser_setParseError_(parser, "requestPath is longer than the allowed length of %d bytes.", MAX_REQUEST_PATH_LENGTH);
		return;
	}
	IoObject_setSlot_to_(self, IOSYMBOL("requestPath"), IOSEQ(buffer, bufferSize));
}

void IoHTTPParser_setQueryString_givenSize_(void *data, const unsigned char * buffer, size_t bufferSize)
{
	IoHTTPParser *self = (IoHTTPParser *)data;
	
	if(bufferSize > MAX_REQUEST_QUERY_STRING_LENGTH)
	{
		HTTPParser *parser = PARSER(self);
		HTTPParser_setParseError_(parser, "queryString is longer than the allowed length of %d bytes.", MAX_REQUEST_QUERY_STRING_LENGTH);
		return;
	}
	IoObject_setSlot_to_(self, IOSYMBOL("queryString"), IOSEQ(buffer, bufferSize));
}

void IoHTTPParser_setHTTPVersion_givenSize_(void *data, const unsigned char * buffer, size_t bufferSize)
{
	IoHTTPParser *self = (IoHTTPParser *)data;
	
	IoObject_setSlot_to_(self, IOSYMBOL("httpVersion"), IOSEQ(buffer, bufferSize));
}

void IoHTTPParser_setBody_givenSize_(void *data, const unsigned char * buffer, size_t bufferSize)
{
	IoHTTPParser *self = (IoHTTPParser *)data;
	
	IoObject_setSlot_to_(self, IOSYMBOL("body"), IOSEQ(buffer, bufferSize));
}