//metadoc XmlReader category Parsers
#include <stdio.h>		//fileno

#include "IoXmlReader.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoList.h"
#include "IoMap.h"
#include "IoFile.h"

#define DATA(self) ((IoXmlReaderData*)IoObject_dataPointer(self))
#define ISOPEN(self) (DATA(self)->xmlFile || DATA(self)->xmlText || DATA(self)->xmlPath)
#define ENSURE_OPEN if (!ISOPEN(self)) IoState_error_(IOSTATE, m, "Call parseFile or parseString first")
#define COPY_ARG_TO(n,o) { o = IoMessage_locals_valueArgAt_(m, locals, n); if (ISNIL(o)) IoState_error_(IOSTATE, m, "Argument %d must be non-nil",n); }
#define REQUIRE_NONOPEN \
	if (ISOPEN(self)) IoState_error_(IOSTATE, m, "Parser properties can only be changed before parsing begins");
#define GETPROP(x) (DATA(self)->x ? DATA(self)->x : IONIL(self))
#define ENSURE_CSTRING(x) (!x || !ISSEQ(x) ? NULL : CSTRING(x))

IoTag *IoXmlReader_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("XmlReader");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoXmlReader_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoXmlReader_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoXmlReader_mark);
	return tag;
}

IoXmlReader *IoXmlReader_rawClone(IoXmlReader *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, cpalloc(IoObject_dataPointer(proto), sizeof(IoXmlReaderData)));
	return self;
}

IoXmlReader *IoXmlReader_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoXmlReader_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(IoXmlReaderData)));

	IoState_registerProtoWithFunc_(state, self, IoXmlReader_proto);
	IoMethodTable methodTable[] = {
		{"parseFile", IoXmlReader_parseFile},
		{"parseString", IoXmlReader_parseString},
		{"setUrl", IoXmlReader_setUrl},
		{"url", IoXmlReader_url},
		{"setEncoding", IoXmlReader_setEncoding},
		{"encoding", IoXmlReader_encoding},
		{"file", IoXmlReader_xmlFile},
		{"xmlText", IoXmlReader_xmlText},
		{"xmlPath", IoXmlReader_xmlPath},
		{"setLoadDtd", IoXmlReader_setLoadDtd},
		{"loadDtd", IoXmlReader_loadDtd},
		{"setDefaultAttrs", IoXmlReader_setDefaultAttrs},
		{"defaultAttrs", IoXmlReader_defaultAttrs},
		{"setSubstEntities", IoXmlReader_setSubstEntities},
		{"substEntities", IoXmlReader_substEntities},
		{"setValidate", IoXmlReader_setValidate},
		{"validate", IoXmlReader_validate},
		{"attributeCount", IoXmlReader_attributeCount},
		{"baseUri", IoXmlReader_baseUri},
		//{"byteConsumed", IoXmlReader_byteConsumed},
		{"close", IoXmlReader_close},
		{"localName", IoXmlReader_localName},
		{"name", IoXmlReader_name},
		{"namespaceUri", IoXmlReader_namespaceUri},
		{"value", IoXmlReader_value},
		{"xmlLang", IoXmlReader_xmlLang},
		{"xmlVersion", IoXmlReader_xmlVersion},
		{"depth",IoXmlReader_depth},
		{"getAttribute",IoXmlReader_getAttribute},
		{"getAttributeNo",IoXmlReader_getAttributeNo},
		{"getAttributeNs",IoXmlReader_getAttributeNs},
		//{"columnNumber",IoXmlReader_columnNumber},
		//{"lineNumber",IoXmlReader_lineNumber},
		{"hasValue",IoXmlReader_hasValue},
		{"hasAttributes",IoXmlReader_hasAttributes},
		{"isDefault", IoXmlReader_isDefault},
		{"isEmptyElement", IoXmlReader_isEmptyElement},
		{"isNamespaceDecl", IoXmlReader_isNamespaceDecl},
		{"isValid", IoXmlReader_isValid},
		{"lookupNamespace", IoXmlReader_lookupNamespace},
		{"moveToAttribute", IoXmlReader_moveToAttribute},
		{"moveToAttributeNo", IoXmlReader_moveToAttributeNo},
		{"moveToAttributeNs", IoXmlReader_moveToAttributeNs},
		{"moveToElement", IoXmlReader_moveToElement},
		{"moveToFirstAttribute", IoXmlReader_moveToFirstAttribute},
		{"moveToNextAttribute", IoXmlReader_moveToNextAttribute},
		{"next", IoXmlReader_next},
		{"nextSibling", IoXmlReader_nextSibling},
		{"nodeType", IoXmlReader_nodeType},
		{"prefix", IoXmlReader_prefix},
		{"quoteChar", IoXmlReader_quoteChar},
		{"read", IoXmlReader_read},
		{"readAttributeValue", IoXmlReader_readAttributeValue},
		{"readInnerXml", IoXmlReader_readInnerXml},
		{"readOuterXml", IoXmlReader_readOuterXml},
		{"readState", IoXmlReader_readState},
		{"readString", IoXmlReader_readString},
		{NULL, NULL}
	};
	IoObject_addMethodTable_(self, methodTable);
	return self;
}

IoXmlReader *IoXmlReader_new(void *state)
{
	return NULL;
}

void IoXmlReader_free(IoXmlReader *self)
{
	if (DATA(self)->reader)
		xmlFreeTextReader(DATA(self)->reader);
	if(DATA(self)->error)
		free(DATA(self)->error);
	free(IoObject_dataPointer(self));
}

void IoXmlReader_mark(IoXmlReader *self)
{
	if (DATA(self)->xmlPath) IoObject_shouldMark((IoObject*)DATA(self)->xmlPath);
	if (DATA(self)->encoding) IoObject_shouldMark((IoObject*)DATA(self)->encoding);
	if (DATA(self)->url) IoObject_shouldMark((IoObject*)DATA(self)->url);
	if (DATA(self)->xmlText) IoObject_shouldMark((IoObject*)DATA(self)->xmlText);
	if (DATA(self)->xmlFile) IoObject_shouldMark((IoObject*)DATA(self)->xmlFile);
}

void IoXmlReader_raiseError(IoXmlReader *self, IoMessage *m)
{
	if(DATA(self)->error)
	{
		IoState_error_(IOSTATE, m, DATA(self)->error);
	}
	else 
	{
		IoState_error_(IOSTATE, m, "error but error callback not called");
	}
}

const char *IoXmlReader_error(IoXmlReader *self)
{ 
	return DATA(self)->error;
}

static void errorFunc(void *arg, const char *msg, xmlParserSeverities severity, xmlTextReaderLocatorPtr locator)
{
	IoXmlReader *self = (IoXmlReader*)arg;
	DATA(self)->error = realloc(DATA(self)->error, strlen(msg) + 1);
	strcpy(DATA(self)->error, msg);
}

/* ------------------------------------------------------------- */
IoObject *IoXmlReader_parseFile(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	IoObject *file_or_filename;
	COPY_ARG_TO(0, file_or_filename);
	
	if (ISFILE(file_or_filename)) 
	{
		DATA(self)->xmlFile = file_or_filename;
		int fd = fileno(((IoFileData*)IoObject_dataPointer(file_or_filename))->stream);
		DATA(self)->reader = xmlReaderForFd(fd,
											ENSURE_CSTRING(DATA(self)->url),
											ENSURE_CSTRING(DATA(self)->encoding),
											DATA(self)->options);
	} 
	else if (ISSEQ(file_or_filename)) 
	{
		DATA(self)->xmlPath = file_or_filename;
		DATA(self)->reader = xmlReaderForFile(ENSURE_CSTRING(file_or_filename),
												ENSURE_CSTRING(DATA(self)->encoding),
												DATA(self)->options);
	} 
	else 
	{
		IoState_error_(IOSTATE, m, "File or Sequence required");
	}
	
	if (!DATA(self)->reader)
		IoState_error_(IOSTATE,m,"Error Initializing Xml Parser");
	xmlTextReaderSetErrorHandler(DATA(self)->reader, errorFunc, self);
	return self;
}

IoObject *IoXmlReader_parseString(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	IoObject *doc;
	COPY_ARG_TO(0, doc);
	if (ISSEQ(doc)) {
		DATA(self)->xmlText = doc;
		DATA(self)->reader = xmlReaderForMemory(ENSURE_CSTRING(doc),
												ENSURE_CSTRING(doc) ? IOSEQ_LENGTH(doc) : 0,
												ENSURE_CSTRING(DATA(self)->url),
												ENSURE_CSTRING(DATA(self)->encoding),
												DATA(self)->options);
	} else {
		IoState_error_(IOSTATE,m, "Sequence required");
	}
	if (!DATA(self)->reader)
		IoState_error_(IOSTATE,m,DATA(self)->error);
	xmlTextReaderSetErrorHandler(DATA(self)->reader, errorFunc, m);
	return self;
}

IoObject *IoXmlReader_setUrl(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	REQUIRE_NONOPEN;
	IoObject *o;
	COPY_ARG_TO(0,o);
	DATA(self)->url = o;
	return self;
}

IoObject *IoXmlReader_url(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	return GETPROP(url);
}

IoObject *IoXmlReader_setEncoding(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	IoObject *o;
	COPY_ARG_TO(0,o);
	DATA(self)->encoding = o;
	return self;
}


IoObject *IoXmlReader_encoding(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	return GETPROP(encoding);
}

IoObject *IoXmlReader_xmlPath(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	return GETPROP(xmlPath);
}

IoObject *IoXmlReader_xmlFile(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	return GETPROP(xmlFile);
}

IoObject *IoXmlReader_xmlText(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	return GETPROP(xmlText);
}

// xmlParserProperties
IoObject *IoXmlReader_setLoadDtd(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	REQUIRE_NONOPEN;
	IoObject *o;
	COPY_ARG_TO(0,o);
	DATA(self)->options = ISTRUE(o) ? DATA(self)->options | XML_PARSER_LOADDTD: DATA(self)->options & ~XML_PARSER_LOADDTD;
	return self;
}

IoObject *IoXmlReader_loadDtd(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	return DATA(self)->options & XML_PARSER_LOADDTD ? IOTRUE(self) : IOFALSE(self);
}

IoObject *IoXmlReader_setDefaultAttrs(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	REQUIRE_NONOPEN;
	IoObject *o;
	COPY_ARG_TO(0,o);
	DATA(self)->options = ISTRUE(o) ? DATA(self)->options | XML_PARSER_DEFAULTATTRS : DATA(self)->options & ~XML_PARSER_DEFAULTATTRS;
	return self;
}

IoObject *IoXmlReader_defaultAttrs(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	return DATA(self)->options & XML_PARSER_DEFAULTATTRS ? IOTRUE(self) : IOFALSE(self);
}

IoObject *IoXmlReader_setValidate(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	REQUIRE_NONOPEN;
	IoObject *o;
	COPY_ARG_TO(0,o);
	DATA(self)->options = ISTRUE(o) ? DATA(self)->options | XML_PARSER_VALIDATE: DATA(self)->options & ~XML_PARSER_VALIDATE;
	return self;
}

IoObject *IoXmlReader_validate(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	return DATA(self)->options & XML_PARSER_VALIDATE ? IOTRUE(self) : IOFALSE(self);
}

IoObject *IoXmlReader_setSubstEntities(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	REQUIRE_NONOPEN;
	IoObject *o;
	COPY_ARG_TO(0,o);
	DATA(self)->options = ISTRUE(o) ? DATA(self)->options | XML_PARSER_SUBST_ENTITIES: DATA(self)->options & ~XML_PARSER_SUBST_ENTITIES;
	return self;
}

IoObject *IoXmlReader_substEntities(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	return DATA(self)->options & XML_PARSER_SUBST_ENTITIES ? IOTRUE(self) : IOFALSE(self);
}

IoObject *IoXmlReader_attributeCount(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	int ret = xmlTextReaderAttributeCount(DATA(self)->reader);
	if (ret < 0) IoXmlReader_raiseError(self, m);
	return IONUMBER(ret);
}

IoObject *IoXmlReader_baseUri(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	xmlChar *s = xmlTextReaderBaseUri(DATA(self)->reader);
	if (s == NULL) return IONIL(self);
	return IOSYMBOL(s);
}

/*
IoObject *IoXmlReader_byteConsumed(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	int ret = xmlTextReaderByteConsumed(DATA(self)->reader);
	if (ret < 0) IoXmlReader_raiseError(self, m);
	return IONUMBER(ret);
}
*/

IoObject *IoXmlReader_close(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	if (xmlTextReaderClose(DATA(self)->reader) == -1)
		IoState_error_(IOSTATE, m, "Couldn't close reader");
	return self;
}

IoObject *IoXmlReader_localName(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	xmlChar *s = xmlTextReaderLocalName(DATA(self)->reader);
	if (s == NULL) return IONIL(self);
	return IOSYMBOL(s);
}

IoObject *IoXmlReader_name(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	xmlChar *s = xmlTextReaderName(DATA(self)->reader);
	if (s == NULL) return IONIL(self);
	return IOSYMBOL(s);
}

IoObject *IoXmlReader_namespaceUri(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	xmlChar *s = xmlTextReaderNamespaceUri(DATA(self)->reader);
	if (s == NULL) return IONIL(self);
	return IOSYMBOL(s);
}

IoObject *IoXmlReader_value(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	xmlChar *s = xmlTextReaderValue(DATA(self)->reader);
	if (s == NULL) return IONIL(self);
	return IOSYMBOL(s);
}

IoObject *IoXmlReader_xmlLang(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	xmlChar *s = xmlTextReaderXmlLang(DATA(self)->reader);
	if (s == NULL) return IONIL(self);
	return IOSYMBOL(s);
}

IoObject *IoXmlReader_xmlVersion(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	const xmlChar *s = xmlTextReaderConstXmlVersion(DATA(self)->reader);
	if (s == NULL) return IONIL(self);
	return IOSYMBOL(s);
}

IoObject *IoXmlReader_depth(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	int ret = xmlTextReaderDepth(DATA(self)->reader);
	if (ret < 0) IoXmlReader_raiseError(self, m);
	return IONUMBER(ret);
}

IoObject *IoXmlReader_getAttribute(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	IoObject *o;
	COPY_ARG_TO(0,o);
	xmlChar *s = xmlTextReaderGetAttribute(DATA(self)->reader, (xmlChar*)ENSURE_CSTRING(o));
	if (s == NULL) return IONIL(self);
	return IOSYMBOL(s);
}

IoObject *IoXmlReader_getAttributeNo(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	IoNumber *num = IoMessage_locals_numberArgAt_(m, locals, 0);
	if (num == NULL) IoState_error_(IOSTATE, m, "Number argument required at 0");
	xmlChar *s = xmlTextReaderGetAttributeNo(DATA(self)->reader, IoNumber_asInt(num));
	if (s == NULL) return IONIL(self);
	return IOSYMBOL(s);
}

IoObject *IoXmlReader_getAttributeNs(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	IoObject *localName, *namespaceURI;
	COPY_ARG_TO(0, localName);
	COPY_ARG_TO(1, namespaceURI);
	xmlChar *s = xmlTextReaderGetAttributeNs(DATA(self)->reader, (xmlChar*)ENSURE_CSTRING(localName), (xmlChar*)ENSURE_CSTRING(namespaceURI));
	if (s == NULL) return IONIL(self);
	return IOSYMBOL(s);
}

/*
IoObject *IoXmlReader_columnNumber(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	int ret = xmlTextReaderGetParserColumnNumber(DATA(self)->reader);
	if (ret < 0) IoXmlReader_raiseError(self, m);
	return IONUMBER(ret);
}
*/

/*
IoObject *IoXmlReader_lineNumber(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	int ret = xmlTextReaderGetParserLineNumber(DATA(self)->reader);
	if (ret < 0) IoXmlReader_raiseError(self, m);
	return IONUMBER(ret);
}
*/

IoObject *IoXmlReader_hasValue(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	int ret = xmlTextReaderHasValue(DATA(self)->reader);
	if (ret < 0) IoXmlReader_raiseError(self, m);
	return ret ? IOTRUE(self) : IOFALSE(self);
}

IoObject *IoXmlReader_hasAttributes(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	int ret = xmlTextReaderHasAttributes(DATA(self)->reader);
	if (ret < 0) IoXmlReader_raiseError(self, m);
	return ret ? IOTRUE(self) : IOFALSE(self);
}

IoObject *IoXmlReader_isDefault(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	int ret = xmlTextReaderIsDefault(DATA(self)->reader);
	if (ret < 0) IoXmlReader_raiseError(self, m);
	return ret ? IOTRUE(self) : IOFALSE(self);
}

IoObject *IoXmlReader_isEmptyElement(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	int ret = xmlTextReaderIsEmptyElement(DATA(self)->reader);
	if (ret < 0) IoXmlReader_raiseError(self, m);
	return ret ? IOTRUE(self) : IOFALSE(self);
}

IoObject *IoXmlReader_isNamespaceDecl(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	int ret = xmlTextReaderIsNamespaceDecl(DATA(self)->reader);
	if (ret < 0) IoXmlReader_raiseError(self, m);
	return ret ? IOTRUE(self) : IOFALSE(self);
}

IoObject *IoXmlReader_isValid(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	int ret = xmlTextReaderIsValid(DATA(self)->reader);
	if (ret < 0) IoXmlReader_raiseError(self, m);
	return ret ? IOTRUE(self) : IOFALSE(self);
}

IoObject *IoXmlReader_lookupNamespace(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	IoObject *o;
	COPY_ARG_TO(0,o);
	xmlChar *s = xmlTextReaderLookupNamespace(DATA(self)->reader, (xmlChar*)ENSURE_CSTRING(o));
	if (s == NULL) return IONIL(self);
	return IOSYMBOL(s);
}

IoObject *IoXmlReader_moveToAttribute(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	IoObject *o;
	COPY_ARG_TO(0,o);
	int ret = xmlTextReaderMoveToAttribute(DATA(self)->reader, (xmlChar*)ENSURE_CSTRING(o));
	if (ret < 0) IoXmlReader_raiseError(self, m);
	return ret == 0 ? IOFALSE(self) : IOTRUE(self);
}

IoObject *IoXmlReader_moveToAttributeNo(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	IoObject *o;
	COPY_ARG_TO(0,o);
	int ret = xmlTextReaderMoveToAttributeNo(DATA(self)->reader, IoNumber_asInt(o));
	if (ret < 0) IoXmlReader_raiseError(self, m);
	return ret == 0 ? IOFALSE(self) : IOTRUE(self);
}

IoObject *IoXmlReader_moveToAttributeNs(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	IoObject *localName, *namespaceURI;
	COPY_ARG_TO(0, localName);
	COPY_ARG_TO(1, namespaceURI);
	int ret = xmlTextReaderMoveToAttributeNs(DATA(self)->reader, (xmlChar*)ENSURE_CSTRING(localName), (xmlChar*)ENSURE_CSTRING(namespaceURI));
	if (ret < 0) IoXmlReader_raiseError(self, m);
	return ret == 0 ? IOFALSE(self) : IOTRUE(self);
}
IoObject *IoXmlReader_moveToElement(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	int ret = xmlTextReaderMoveToElement(DATA(self)->reader);
	if (ret < 0) IoXmlReader_raiseError(self, m);
	return ret == 0 ? IOFALSE(self) : IOTRUE(self);
}

IoObject *IoXmlReader_moveToFirstAttribute(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	int ret = xmlTextReaderMoveToFirstAttribute(DATA(self)->reader);
	if (ret < 0) IoXmlReader_raiseError(self, m);
	return ret == 0 ? IOFALSE(self) : IOTRUE(self);
}

IoObject *IoXmlReader_moveToNextAttribute(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	int ret = xmlTextReaderMoveToNextAttribute(DATA(self)->reader);
	if (ret < 0) IoXmlReader_raiseError(self, m);
	return ret == 0 ? IOFALSE(self) : IOTRUE(self);
}

IoObject *IoXmlReader_next(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	int ret = xmlTextReaderNext(DATA(self)->reader);
	if (ret < 0) IoXmlReader_raiseError(self, m);
	return ret == 0 ? IOFALSE(self) : IOTRUE(self);
}

IoObject *IoXmlReader_nextSibling(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	int ret = xmlTextReaderNextSibling(DATA(self)->reader);
	if (ret < 0) IoXmlReader_raiseError(self, m);
	return ret == 0 ? IOFALSE(self) : IOTRUE(self);
}

IoObject *IoXmlReader_nodeType(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	//printf("\nnodeType begin\n");
	ENSURE_OPEN;
	int ret = xmlTextReaderNodeType(DATA(self)->reader);
	if (ret < 0) 
	{
		printf("ERROR!!!!!!!!\n");
		IoState_error_(IOSTATE, m, DATA(self)->error);
	}
	
	switch (ret) {
		case XML_READER_TYPE_NONE: return IOSYMBOL("NONE");
		case XML_READER_TYPE_ELEMENT: return IOSYMBOL("ELEMENT");
		case XML_READER_TYPE_ATTRIBUTE: return IOSYMBOL("ATTRIBUTE");
		case XML_READER_TYPE_TEXT: return IOSYMBOL("TEXT");
		case XML_READER_TYPE_CDATA: return IOSYMBOL("CDATA");
		case XML_READER_TYPE_ENTITY_REFERENCE: return IOSYMBOL("ENTITY_REFERENCE");
		case XML_READER_TYPE_ENTITY: return IOSYMBOL("ENTITY");
		case XML_READER_TYPE_PROCESSING_INSTRUCTION: return IOSYMBOL("PROCESSING_INSTRUCTION");
		case XML_READER_TYPE_COMMENT: return IOSYMBOL("COMMENT");
		case XML_READER_TYPE_DOCUMENT: return IOSYMBOL("DOCUMENT");
		case XML_READER_TYPE_DOCUMENT_TYPE: return IOSYMBOL("DOCUMENT_TYPE");
		case XML_READER_TYPE_DOCUMENT_FRAGMENT: return IOSYMBOL("DOCUMENT_FRAGMENT");
		case XML_READER_TYPE_NOTATION: return IOSYMBOL("NOTATION");
		case XML_READER_TYPE_WHITESPACE: return IOSYMBOL("WHITESPACE");
		case XML_READER_TYPE_SIGNIFICANT_WHITESPACE: return IOSYMBOL("SIGNIFICANT_WHITESPACE");
		case XML_READER_TYPE_END_ELEMENT: return IOSYMBOL("END_ELEMENT");
		case XML_READER_TYPE_END_ENTITY: return IOSYMBOL("END_ENTITY");
		case XML_READER_TYPE_XML_DECLARATION: return IOSYMBOL("XML_DECLARATION");
		default: IoState_error_(IOSTATE, m, "Unknown return code from xmlTextReaderNodeType");
	}

	return IONIL(self);		// suppress error
}

IoObject *IoXmlReader_prefix(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	xmlChar *s = xmlTextReaderPrefix(DATA(self)->reader);
	if (s == NULL) return IONIL(self);
	return IOSYMBOL(s);
}

IoObject *IoXmlReader_quoteChar(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	int ret = xmlTextReaderQuoteChar(DATA(self)->reader);
	if (ret < 0) IoXmlReader_raiseError(self, m);
	return IONUMBER(ret);
}

IoObject *IoXmlReader_read(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	int ret = xmlTextReaderRead(DATA(self)->reader);
	if (ret < 0) IoXmlReader_raiseError(self, m);
	return ret == 0 ? IOFALSE(self) : IOTRUE(self);
}

IoObject *IoXmlReader_readAttributeValue(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	int ret = xmlTextReaderReadAttributeValue(DATA(self)->reader);
	if (ret < 0) IoXmlReader_raiseError(self, m);
	return ret == 0 ? IOFALSE(self) : IOTRUE(self);
}

IoObject *IoXmlReader_readInnerXml(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	xmlChar *s = xmlTextReaderReadInnerXml(DATA(self)->reader);
	if (s == NULL) return IONIL(self);
	else {
		IoObject *ret = IOSYMBOL(s);
		xmlFree(s);
		return ret;
	}
}

IoObject *IoXmlReader_readOuterXml(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	xmlChar *s = xmlTextReaderReadOuterXml(DATA(self)->reader);
	if (s == NULL) return IONIL(self);
	else {
		IoObject *ret = IOSYMBOL(s);
		xmlFree(s);
		return ret;
	}
}

IoObject *IoXmlReader_readState(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	int ret = xmlTextReaderReadState(DATA(self)->reader);
	if (ret < 0) IoXmlReader_raiseError(self, m);
	return IONUMBER(ret);
}

IoObject *IoXmlReader_readString(IoXmlReader *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	xmlChar *s = xmlTextReaderReadString(DATA(self)->reader);
	if (s == NULL) return IONIL(self);
	IoObject *ret = IOSYMBOL(s);
	xmlFree(s);
	return ret;
}


