#include "IoHttpParser.h"
#include "IoMap.h"
#include "IoError.h"
#include "http11_parser.h"

#undef TRUE
#define TRUE    1               /* boolean true */
#undef FALSE
#define FALSE   0               /* boolean false */

#define PARSER(self) (HttpParser *)IoObject_dataPointer(self)
#define IOSEQDATA(self) ((UArray *)(IoObject_dataPointer(self)))

static const char *protoId = "HttpParser";

IoTag *IoHttpParser_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoHttpParser_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoHttpParser_rawClone);
	return tag;
}

IoHttpParser *IoHttpParser_proto(void *state)
{
	IoObject *self = IoObject_new(state);

	IoObject_tag_(self, IoHttpParser_newTag(state));

	IoState_registerProtoWithId_(state, self, protoId);
	
	IoHttpParser_initState(self);

	{
		IoMethodTable methodTable[] = {
			{"rawParse", IoHttpParser_rawParse},
			{"isFinished", IoHttpParser_isFinished},
			{"rawReset", IoHttpParser_rawReset},
			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoHttpParser *IoHttpParser_rawClone(IoHttpParser *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);

	IoHttpParser_initState(self);

	return self;
}

IoHttpParser *IoHttpParser_new(void *state)
{
	IoHttpParser *proto = IoState_protoWithId_(state, protoId);
	return IOCLONE(proto);
}

void IoHttpParser_free(IoHttpParser *self)
{
	IoHttpParser_freeParser(self);
}

void IoHttpParser_initState(IoHttpParser *self)
{
	IoHttpParser_initParser(self);

	IoObject_setSlot_to_(self, IOSYMBOL("httpFields"), IoMap_new(IOSTATE));
	IoObject_setSlot_to_(self, IOSYMBOL("requestUri"), IoSeq_newWithCString_(IOSTATE, ""));
	IoObject_setSlot_to_(self, IOSYMBOL("fragment"), IoSeq_newWithCString_(IOSTATE, ""));
	IoObject_setSlot_to_(self, IOSYMBOL("requestPath"), IoSeq_newWithCString_(IOSTATE, ""));
	IoObject_setSlot_to_(self, IOSYMBOL("queryString"), IoSeq_newWithCString_(IOSTATE, ""));
	IoObject_setSlot_to_(self, IOSYMBOL("httpVersion"), IoSeq_newWithCString_(IOSTATE, ""));
	IoObject_setSlot_to_(self, IOSYMBOL("requestMethod"), IoSeq_newWithCString_(IOSTATE, ""));
	IoObject_setSlot_to_(self, IOSYMBOL("body"), IoSeq_newWithCString_(IOSTATE, ""));
}

void IoHttpParser_freeParser(IoHttpParser *self)
{
	HttpParser_free(PARSER(self));
}

void IoHttpParser_initParser(IoHttpParser *self)
{
	HttpParser *parser = HttpParser_new();

	HttpParser_setHttpFieldCallback_(parser, (field_cb) IoHttpParser_setHttpField_withName_givenSize_value_givenSize_);
	HttpParser_setRequestURICallback_(parser, (element_cb) IoHttpParser_setRequestURI_givenSize_);
	HttpParser_setFragmentCallback_(parser, (element_cb) IoHttpParser_setFragment_givenSize_);
	HttpParser_setRequestPathCallback_(parser, (element_cb) IoHttpParser_setRequestPath_givenSize_);
	HttpParser_setQueryStringCallback_(parser, (element_cb) IoHttpParser_setQueryString_givenSize_);
	HttpParser_setHttpVersionCallback_(parser, (element_cb) IoHttpParser_setHttpVersion_givenSize_);
	HttpParser_setRequestMethodCallback_(parser, (element_cb) IoHttpParser_setRequestMethod_givenSize_);
	HttpParser_setHeaderDoneCallback_(parser, (element_cb) IoHttpParser_setBody_givenSize_);
	IoObject_setDataPointer_(self, parser);
}

/* ----------------------------------------------------------- */

IoHttpParser *IoHttpParser_rawParse(IoHttpParser *self, IoObject *locals, IoMessage *m)
{
	//doc HttpParser parse Parsers the Http request stored in the parseBuffer slot.  Sets slots on self using the values parsed.  Returns self if successful or an Error if the parse fails.
	HttpParser *parser = PARSER(self);
	IoSeq *parseBuffer = IoObject_getSlot_(self, IOSYMBOL("parseBuffer"));
	IoMap *httpFields = IoObject_getSlot_(self, IOSYMBOL("httpFields"));
	
	IOASSERT(ISSEQ(parseBuffer), "The parseBuffer should be set to a Sequence before parse is called.");
	IOASSERT(IoSeq_rawSize(parseBuffer) > 0, "The parseBuffer can't be empty when parse is called.");
	
	IOASSERT(ISMAP(httpFields), "httpFields should be set to a Map before parse is called.");

	HttpParser_parse_fromBuffer_givenLength_(parser, self, IoSeq_asCString(parseBuffer), IoSeq_rawSizeInBytes(parseBuffer));
	
	if (HttpParser_bytesParsed(parser) > MAX_HEADER_LENGTH)
	{
		return IoError_newWithMessageFormat_(IOSTATE, "Header is longer than the allowed length of %d bytes.", MAX_HEADER_LENGTH);
	}

	if (HttpParser_parseErrorOccured(parser))
	{
		UArray *parseError = HttpParser_parseError(parser);
		if (parseError)
		{
			return IoError_newWithMessageFormat_(IOSTATE, UArray_asCString(parseError));
		}
		else
		{
			return IoError_newWithMessageFormat_(IOSTATE, "Invalid Http format.  Parse failed.");
		}
	}

	return self;
}

IoHttpParser *IoHttpParser_isFinished(IoHttpParser *self, IoObject *locals, IoMessage *m)
{
	//doc HttpParser isFinished Returns true if the parser has fully completed parsing of the content in the parse buffer and false if it hasn't.
	HttpParser *parser = PARSER(self);

	return IOBOOL(self, HttpParser_isFinished(parser));
}

IoHttpParser *IoHttpParser_rawReset(IoHttpParser *self, IoObject *locals, IoMessage *m)
{
	//doc HttpParser rawReset Resets the parser state to prepare for a new parse.  Returns self.
	IoHttpParser_freeParser(self);
	IoHttpParser_initParser(self);
	return self;
}

/* ----------------------------------------------------------- */

void IoHttpParser_setHttpField_withName_givenSize_value_givenSize_(void *data, const unsigned char *fieldNameBuffer, size_t fieldNameSize, const unsigned char *fieldValueBuffer, size_t fieldValueSize)
{
	IoHttpParser *self = (IoHttpParser *)data;
	
	if (fieldNameSize > MAX_FIELD_NAME_LENGTH)
	{
		HttpParser *parser = PARSER(self);
		HttpParser_setParseError_(parser, "Http field name is longer than the allowed length of %d bytes.", MAX_FIELD_NAME_LENGTH);
		return;
	}

	if (fieldValueSize > MAX_FIELD_VALUE_LENGTH)
	{
		HttpParser *parser = PARSER(self);
		HttpParser_setParseError_(parser, "Http field value is longer than the allowed length of %d bytes.", MAX_FIELD_VALUE_LENGTH);
		return;
	}

	{
		IoMap *httpFields = IoObject_getSlot_(self, IOSYMBOL("httpFields"));
		IoSeq *fieldValue = IOSEQ(fieldValueBuffer, fieldValueSize);
		UArray *fieldName = UArray_newWithData_type_size_copy_((char *)fieldNameBuffer, CTYPE_uint8_t, fieldNameSize, 1);
		UArray_toupper(fieldName);
		UArray_replaceCString_withCString_(fieldName, "-", "_");

		//UArray_toupper(fieldName);
		//UArray_replaceCString_withCString_(fieldName, "-", "_");

		//IoObject_setSlot_to_(self, IoState_symbolWithUArray_copy_((IoState*)IOSTATE, fieldName, 0), fieldValue);
		IoMap_rawAtPut(httpFields, IoState_symbolWithUArray_copy_((IoState*)IOSTATE, fieldName, 0), fieldValue);
	}
}

void IoHttpParser_setRequestURI_givenSize_(void *data, const unsigned char * buffer, size_t bufferSize)
{
	IoHttpParser *self = (IoHttpParser *)data;
	
	if(bufferSize > MAX_REQUEST_URI_LENGTH)
	{
		HttpParser *parser = PARSER(self);
		HttpParser_setParseError_(parser, "requestURI is longer than the allowed length of %d bytes.", MAX_REQUEST_URI_LENGTH);
		return;
	}
	
	{
		IoSeq *requestURI = IoObject_getSlot_(self, IOSYMBOL("requestUri"));
		UArray_setData_type_size_copy_(IOSEQDATA(requestURI), (void *)buffer, CTYPE_uint8_t, bufferSize, 1);
	}
}

void IoHttpParser_setFragment_givenSize_(void *data, const unsigned char * buffer, size_t bufferSize)
{
	IoHttpParser *self = (IoHttpParser *)data;
	
	if(bufferSize > MAX_REQUEST_FRAGMENT_LENGTH)
	{
		HttpParser *parser = PARSER(self);
		HttpParser_setParseError_(parser, "fragment is longer than the allowed length of %d bytes.", MAX_REQUEST_FRAGMENT_LENGTH);
		return;
	}
	
	{
		IoSeq *fragment = IoObject_getSlot_(self, IOSYMBOL("fragment"));
		UArray_setData_type_size_copy_(IOSEQDATA(fragment), (void *)buffer, CTYPE_uint8_t, bufferSize, 1);
	}
}

void IoHttpParser_setRequestPath_givenSize_(void *data, const unsigned char * buffer, size_t bufferSize)
{
	IoHttpParser *self = (IoHttpParser *)data;
	
	if(bufferSize > MAX_REQUEST_PATH_LENGTH)
	{
		HttpParser *parser = PARSER(self);
		HttpParser_setParseError_(parser, "requestPath is longer than the allowed length of %d bytes.", MAX_REQUEST_PATH_LENGTH);
		return;
	}
	
	{
		IoSeq *requestPath = IoObject_getSlot_(self, IOSYMBOL("requestPath"));
		UArray_setData_type_size_copy_(IOSEQDATA(requestPath), (void *)buffer, CTYPE_uint8_t, bufferSize, 1);
	}
}

void IoHttpParser_setQueryString_givenSize_(void *data, const unsigned char * buffer, size_t bufferSize)
{
	IoHttpParser *self = (IoHttpParser *)data;
	
	if(bufferSize > MAX_REQUEST_QUERY_STRING_LENGTH)
	{
		HttpParser *parser = PARSER(self);
		HttpParser_setParseError_(parser, "queryString is longer than the allowed length of %d bytes.", MAX_REQUEST_QUERY_STRING_LENGTH);
		return;
	}
	
	{
		IoSeq *queryString = IoObject_getSlot_(self, IOSYMBOL("queryString"));
		UArray_setData_type_size_copy_(IOSEQDATA(queryString), (void *)buffer, CTYPE_uint8_t, bufferSize, 1);
	}
}

void IoHttpParser_setHttpVersion_givenSize_(void *data, const unsigned char * buffer, size_t bufferSize)
{
	IoHttpParser *self = (IoHttpParser *)data;
	
	{
		IoSeq *httpVersion = IoObject_getSlot_(self, IOSYMBOL("httpVersion"));
		UArray_setData_type_size_copy_(IOSEQDATA(httpVersion), (void *)buffer, CTYPE_uint8_t, bufferSize, 1);
	}
}

void IoHttpParser_setRequestMethod_givenSize_(void *data, const unsigned char * buffer, size_t bufferSize)
{
	IoHttpParser *self = (IoHttpParser *)data;
	
	{
		IoSeq *requestMethod = IoObject_getSlot_(self, IOSYMBOL("requestMethod"));
		UArray_setData_type_size_copy_(IOSEQDATA(requestMethod), (void *)buffer, CTYPE_uint8_t, bufferSize, 1);
	}
}

void IoHttpParser_setBody_givenSize_(void *data, const unsigned char * buffer, size_t bufferSize)
{
	IoHttpParser *self = (IoHttpParser *)data;
	
	{
		IoSeq *body = IoObject_getSlot_(self, IOSYMBOL("body"));
		UArray_setData_type_size_copy_(IOSEQDATA(body), (void *)buffer, CTYPE_uint8_t, bufferSize, 1);
	}
}