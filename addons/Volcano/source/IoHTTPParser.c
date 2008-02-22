#include "IoHTTPParser.h"
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
	HTTPParser *parser = HTTPParser_new();

	IoObject_tag_(self, IoHTTPParser_newTag(state));

	HTTPParser_setHTTPFieldCallback_(parser, (field_cb) IoHTTPParser_setHTTPField_withName_givenSize_value_givenSize_);
	HTTPParser_setRequestURICallback_(parser, (element_cb) IoHTTPParser_setRequestURI_givenSize_);
	HTTPParser_setFragmentCallback_(parser, (element_cb) IoHTTPParser_setFragment_givenSize_);
	HTTPParser_setRequestPathCallback_(parser, (element_cb) IoHTTPParser_setRequestPath_givenSize_);
	HTTPParser_setQueryStringCallback_(parser, (element_cb) IoHTTPParser_setQueryString_givenSize_);
	HTTPParser_setHTTPVersionCallback_(parser, (element_cb) IoHTTPParser_setHTTPVersion_givenSize_);
	IoObject_setDataPointer_(self, parser);

	IoState_registerProtoWithFunc_(state, self, IoHTTPParser_proto);

	{
		IoMethodTable methodTable[] = {
		{"parse", IoHTTPParser_parse},
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoHTTPParser *IoHTTPParser_rawClone(IoHTTPParser *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
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

/* ----------------------------------------------------------- */

IoHTTPParser *IoHTTPParser_parse(IoHTTPParser *self, IoObject *locals, IoMessage *m)
{
	//doc HTTPParser parse Parsers the HTTP request stored in the parseBuffer slot.  Sets slots on self using the values parsed.  Returns self if successful or an Error if the parse fails.
	HTTPParser *parser = PARSER(self);
	IoSeq *parseBuffer = IoObject_getSlot_(self, IOSYMBOL("parseBuffer"));
	
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

/* ----------------------------------------------------------- */

void IoHTTPParser_setHTTPField_withName_givenSize_value_givenSize_(void *data, const unsigned char *fieldNameBuffer, size_t fieldNameSize, const unsigned char *fieldValueBuffer, size_t fieldValueSize)
{
	IoHTTPParser *self = (IoHTTPParser *)data;
	if(fieldNameSize > MAX_FIELD_NAME_LENGTH)
	{
		HTTPParser *parser = PARSER(self);
		HTTPParser_setParseError_(parser, "HTTP field name is longer than the allowed length of %d bytes.", MAX_FIELD_NAME_LENGTH);
		return;
	}

	if(fieldValueSize > MAX_FIELD_VALUE_LENGTH)
	{
		HTTPParser *parser = PARSER(self);
		HTTPParser_setParseError_(parser, "HTTP field value is longer than the allowed length of %d bytes.", MAX_FIELD_VALUE_LENGTH);
		return;
	}

	{
		UArray *fieldName = UArray_newWithData_type_size_copy_(fieldNameBuffer, CTYPE_uint8_t, fieldNameSize, TRUE);
		IoSeq *fieldValue = IOSEQ(fieldValueBuffer, fieldValueSize);

		UArray_toupper(fieldName);
		UArray_replaceCString_withCString_(fieldName, "-", "_");

		IoObject_setSlot_to_(self, IoState_symbolWithUArray_copy_((IoState*)IOSTATE, fieldName, FALSE), fieldValue);
	}
}

void IoHTTPParser_setRequestURI_givenSize_(void *data, const unsigned char * buffer, size_t bufferSize)
{
	IoHTTPParser *self = (IoHTTPParser *)data;
	
	if(bufferSize > MAX_REQUEST_URI_LENGTH)
	{
		HTTPParser *parser = PARSER(self);
		HTTPParser_setParseError_(parser, "REQUEST_URI is longer than the allowed length of %d bytes.", MAX_REQUEST_URI_LENGTH);
		return;
	}
	
	IoObject_setSlot_to_(self, IOSYMBOL("REQUEST_URI"), IOSEQ(buffer, bufferSize));
}

void IoHTTPParser_setFragment_givenSize_(void *data, const unsigned char * buffer, size_t bufferSize)
{
	IoHTTPParser *self = (IoHTTPParser *)data;
	
	if(bufferSize > MAX_REQUEST_FRAGMENT_LENGTH)
	{
		HTTPParser *parser = PARSER(self);
		HTTPParser_setParseError_(parser, "FRAGMENT is longer than the allowed length of %d bytes.", MAX_REQUEST_FRAGMENT_LENGTH);
		return;
	}
	IoObject_setSlot_to_(self, IOSYMBOL("FRAGMENT"), IOSEQ(buffer, bufferSize));
}

void IoHTTPParser_setRequestPath_givenSize_(void *data, const unsigned char * buffer, size_t bufferSize)
{
	IoHTTPParser *self = (IoHTTPParser *)data;
	
	if(bufferSize > MAX_REQUEST_PATH_LENGTH)
	{
		HTTPParser *parser = PARSER(self);
		HTTPParser_setParseError_(parser, "REQUEST_PATH is longer than the allowed length of %d bytes.", MAX_REQUEST_PATH_LENGTH);
		return;
	}
	IoObject_setSlot_to_(self, IOSYMBOL("REQUEST_PATH"), IOSEQ(buffer, bufferSize));
}

void IoHTTPParser_setQueryString_givenSize_(void *data, const unsigned char * buffer, size_t bufferSize)
{
	IoHTTPParser *self = (IoHTTPParser *)data;
	
	if(bufferSize > MAX_REQUEST_QUERY_STRING_LENGTH)
	{
		HTTPParser *parser = PARSER(self);
		HTTPParser_setParseError_(parser, "QUERY_STRING is longer than the allowed length of %d bytes.", MAX_REQUEST_QUERY_STRING_LENGTH);
		return;
	}
	IoObject_setSlot_to_(self, IOSYMBOL("QUERY_STRING"), IOSEQ(buffer, bufferSize));
}

void IoHTTPParser_setHTTPVersion_givenSize_(void *data, const unsigned char * buffer, size_t bufferSize)
{
	IoHTTPParser *self = (IoHTTPParser *)data;
	
	IoObject_setSlot_to_(self, IOSYMBOL("HTTP_VERSION"), IOSEQ(buffer, bufferSize));
}