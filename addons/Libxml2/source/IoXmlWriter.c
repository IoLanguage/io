//metadoc XmlWriter category Parsers

#include <stdio.h>		//fileno

#include "IoXmlWriter.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoList.h"
#include "IoMap.h"
#include "IoFile.h"

#define ENSURE_OPEN if (!ISOPEN(self)) IoState_error_(IOSTATE, m, "Call openFile or open first")
#define ISOPEN(self) (DATA(self)->writer)
#define DATA(self) ((IoXmlWriterData*)IoObject_dataPointer(self))

static const char *protoId = "XmlWriter";

void IoXmlWriter_raiseErrors_(IoXmlWriter *self, IoMessage *m, int rc)
{
	if ( rc < 0 )
	{
		IoState_error_(IOSTATE, m, "Error from underlying XmlTextWriter", rc);
	}
}

IoTag *IoXmlWriter_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoXmlWriter_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoXmlWriter_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoXmlWriter_mark);
	return tag;
}

IoXmlWriter *IoXmlWriter_rawClone(IoXmlWriter *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, cpalloc(IoObject_dataPointer(proto), sizeof(IoXmlWriterData)));
	return self;
}

IoXmlWriter *IoXmlWriter_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoXmlWriter_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(IoXmlWriterData)));

	IoState_registerProtoWithId_(state, self, protoId);
	IoMethodTable methodTable[] = {
		{"openFilename", IoXmlWriter_openFilename},
		{"openStandardOut", IoXmlWriter_openStandardOut},
		{"openFd", IoXmlWriter_openFd},
		{"open", IoXmlWriter_open},
		{"content", IoXmlWriter_content},
		{"close", IoXmlWriter_close},
		{"flush", IoXmlWriter_flush},

		{"setIndent", IoXmlWriter_setIndent},
		{"setIndentString", IoXmlWriter_setIndentString},

		{"startDocument", IoXmlWriter_startDocument},
		{"endDocument", IoXmlWriter_endDocument},

		{"startComment", IoXmlWriter_startComment},
		{"endComment", IoXmlWriter_endComment},
		{"writeComment", IoXmlWriter_writeComment},

		{"startElement", IoXmlWriter_startElement},
		{"startElementNS", IoXmlWriter_startElementNS},
		{"endElement", IoXmlWriter_endElement},
		{"fullEndElement", IoXmlWriter_fullEndElement},

		{"writeElement", IoXmlWriter_writeElement},
		{"writeElementNS", IoXmlWriter_writeElementNS},

		{"writeRaw", IoXmlWriter_writeRaw},
		{"writeString", IoXmlWriter_writeString},
		{"writeBase64", IoXmlWriter_writeBase64},
		{"writeBinHex", IoXmlWriter_writeBinHex},

		{"startAttribute", IoXmlWriter_startAttribute},
		{"startAttributeNS", IoXmlWriter_startAttributeNS},
		{"endAttribute", IoXmlWriter_endAttribute},

		{"writeAttribute", IoXmlWriter_writeAttribute},
		{"writeAttributeNS", IoXmlWriter_writeAttributeNS},

		{"startPI", IoXmlWriter_startPI},
		{"endPI", IoXmlWriter_endPI},
		{"writePI", IoXmlWriter_writePI},

		{"startCDATA", IoXmlWriter_startCDATA},
		{"endCDATA", IoXmlWriter_endCDATA},
		{"writeCDATA", IoXmlWriter_writeCDATA},

		{"startDTD", IoXmlWriter_startDTD},
		{"endDTD", IoXmlWriter_endDTD},
		{"writeDTD", IoXmlWriter_writeDTD},

		{"startDTDElement", IoXmlWriter_startDTDElement},
		{"endDTDElement", IoXmlWriter_endDTDElement},
		{"writeDTDElement", IoXmlWriter_writeDTDElement},

		{"startDTDAttlist", IoXmlWriter_startDTDAttlist},
		{"endDTDAttlist", IoXmlWriter_endDTDAttlist},
		{"writeDTDAttlist", IoXmlWriter_writeDTDAttlist},

		{"writeStartDTDEntity", IoXmlWriter_writeStartDTDEntity},
		{"writeEndDTDEntity", IoXmlWriter_writeEndDTDEntity},
		{"writeDTDInternalEntity", IoXmlWriter_writeDTDInternalEntity},
		{"writeDTDExternalEntity", IoXmlWriter_writeDTDExternalEntity},
		{"writeDTDExternalEntityContents", IoXmlWriter_writeDTDExternalEntityContents},
		{"writeDTDEntity", IoXmlWriter_writeDTDEntity},
		{"writeDTDNotation", IoXmlWriter_writeDTDNotation},

		{NULL, NULL}
	};
	IoObject_addMethodTable_(self, methodTable);
	return self;
}

IoXmlWriter *IoXmlWriter_new(void *state)
{
	IoXmlWriter *proto = IoState_protoWithId_(state, protoId);
	return IOCLONE(proto);
}

void IoXmlWriter_free(IoXmlWriter *self)
{
	if (DATA(self)->writer)
	{
		int rc = xmlTextWriterEndDocument(DATA(self)->writer);
		(void)rc;
		// TODO: handle error
		// quag: Do we care if this errors? we're trying to dispose
		xmlFreeTextWriter(DATA(self)->writer);
	}
	if (DATA(self)->buffer)
	{
		xmlBufferFree(DATA(self)->buffer);
	}
	free(IoObject_dataPointer(self));
}

void IoXmlWriter_mark(IoXmlWriter *self)
{
}

/* ------------------------------------------------------------- */

char *IoMessage_locals_cStringArgAtOrNULL_(IoMessage *self, IoObject *locals, int n)
{
	IoObject *value = IoMessage_locals_valueArgAt_(self, locals, n);
	if (value == IOSTATE->ioNil)
	{
		return NULL;
	}
	if (!ISSEQ(value))
	{
		IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "Sequence");
	}
	return CSTRING(value);
}

char *IoMessage_locals_cStringArgAtOrNULLSize_(IoMessage *self, IoObject *locals, int n, int *size)
{
	IoObject *value = IoMessage_locals_valueArgAt_(self, locals, n);
	if (value == IOSTATE->ioNil)
	{
		*size = 0;
		return NULL;
	}
	if (!ISSEQ(value))
	{
		IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "Sequence");
	}
	*size = IOSEQ_LENGTH(value);
	return CSTRING(value);
}

/* ------------------------------------------------------------- */

IoObject *IoXmlWriter_openFilename(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	IoSymbol *path = IoMessage_locals_symbolArgAt_(m, locals, 0);
	DATA(self)->writer = xmlNewTextWriterFilename(CSTRING(path), 0);
	return self;
}

IoObject *IoXmlWriter_openStandardOut(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	xmlCharEncodingHandlerPtr encoding = xmlFindCharEncodingHandler("UTF-8");
	IOASSERT(encoding, "Encoding is null.");
	xmlOutputBufferPtr buffer = xmlOutputBufferCreateFile(stdout, encoding);
	IOASSERT(buffer, "buffer is null.");
	DATA(self)->writer = xmlNewTextWriter(buffer);
	return self;
}

IoObject *IoXmlWriter_openFd(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	int fd = IoMessage_locals_intArgAt_(m, locals, 0);

	xmlCharEncodingHandlerPtr encoding = xmlFindCharEncodingHandler("UTF-8");
	IOASSERT(encoding, "Encoding is null.");
	xmlOutputBufferPtr buffer = xmlOutputBufferCreateFd(fd, encoding);
	IOASSERT(buffer, "buffer is null.");
	DATA(self)->writer = xmlNewTextWriter(buffer);
	return self;
}

IoObject *IoXmlWriter_open(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	DATA(self)->writer = xmlNewTextWriterMemory(DATA(self)->buffer = xmlBufferCreate(), 0);
	return self;
}

IoObject *IoXmlWriter_content(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	IoXmlWriter_flush(self, locals, m);
	xmlTextWriterFlush(DATA(self)->writer);
	if (DATA(self)->buffer == NULL) return IONIL(self);
	return IOSYMBOL(xmlBufferContent(DATA(self)->buffer));
}

IoObject *IoXmlWriter_startDocument(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	const char *version = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 0);
	const char *encoding = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 1);
	const char *standalone = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 2);

	ENSURE_OPEN;
	int rc = xmlTextWriterStartDocument(DATA(self)->writer, version, encoding, standalone);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_endDocument(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	int rc = xmlTextWriterEndDocument(DATA(self)->writer);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_close(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	xmlFreeTextWriter(DATA(self)->writer);
	DATA(self)->writer = NULL;
	return self;
}

IoObject *IoXmlWriter_startComment(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	int rc = xmlTextWriterStartComment(DATA(self)->writer);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_endComment(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	int rc = xmlTextWriterEndComment(DATA(self)->writer);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_writeComment(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	const char *comment = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 0);

	ENSURE_OPEN;
	int rc = xmlTextWriterWriteComment(DATA(self)->writer, (xmlChar*)comment);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_startElement(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	const char *name = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 0);

	ENSURE_OPEN;
	int rc = xmlTextWriterStartElement(DATA(self)->writer, (xmlChar*)name);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_startElementNS(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	const char *prefix = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 0);
	const char *name = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 1);
	const char *namespaceURI = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 2);

	ENSURE_OPEN;
	int rc = xmlTextWriterStartElementNS(DATA(self)->writer, (xmlChar*)prefix, (xmlChar*)name, (xmlChar*)namespaceURI);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_endElement(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	int rc = xmlTextWriterEndElement(DATA(self)->writer);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_fullEndElement(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	int rc = xmlTextWriterFullEndElement(DATA(self)->writer);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_writeElement(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	const char *name = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 0);
	const char *content = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 1);

	ENSURE_OPEN;
	xmlResetLastError();
	int rc = xmlTextWriterWriteElement(DATA(self)->writer, (xmlChar*)name, (xmlChar*)content);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_writeElementNS(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	const char *prefix = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 0);
	const char *name = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 1);
	const char *namespaceURI = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 2);
	const char *content = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 3);

	ENSURE_OPEN;
	int rc = xmlTextWriterWriteElementNS(DATA(self)->writer, (xmlChar*)prefix, (xmlChar*)name, (xmlChar*)namespaceURI, (xmlChar*)content);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_writeRaw(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	const char *content = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 0);

	ENSURE_OPEN;
	int rc = xmlTextWriterWriteRaw(DATA(self)->writer, (xmlChar*)content);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_writeString(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	const char *content = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 0);

	ENSURE_OPEN;
	int rc = xmlTextWriterWriteString(DATA(self)->writer, (xmlChar*)content);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_writeBase64(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	const int start = 0;
	int len = 0;
	const char *content = IoMessage_locals_cStringArgAtOrNULLSize_(m, locals, 0, &len);

	ENSURE_OPEN;
	int rc = xmlTextWriterWriteBase64(DATA(self)->writer, content, start, len);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_writeBinHex(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	const int start = 0;
	int len = 0;
	const char *content = IoMessage_locals_cStringArgAtOrNULLSize_(m, locals, 0, &len);

	ENSURE_OPEN;
	int rc = xmlTextWriterWriteBinHex(DATA(self)->writer, content, start, len);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_startAttribute(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	const char *name = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 0);

	ENSURE_OPEN;
	int rc = xmlTextWriterStartAttribute(DATA(self)->writer, (xmlChar*)name);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_startAttributeNS(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	const char *prefix = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 0);
	const char *name = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 1);
	const char *namespaceURI = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 2);

	ENSURE_OPEN;
	int rc = xmlTextWriterStartAttributeNS(DATA(self)->writer, (xmlChar*)prefix, (xmlChar*)name, (xmlChar*)namespaceURI);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_endAttribute(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	int rc = xmlTextWriterEndAttribute(DATA(self)->writer);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_writeAttribute(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	const char *name = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 0);
	const char *content = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 1);

	ENSURE_OPEN;
	int rc = xmlTextWriterWriteAttribute(DATA(self)->writer, (xmlChar*)name, (xmlChar*)content);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_writeAttributeNS(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	const char *prefix = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 0);
	const char *name = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 1);
	const char *namespaceURI = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 2);
	const char *content = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 3);

	ENSURE_OPEN;
	int rc = xmlTextWriterWriteAttributeNS(DATA(self)->writer, (xmlChar*)prefix, (xmlChar*)name, (xmlChar*)namespaceURI, (xmlChar*)content);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_startPI(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	const char *target = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 0);

	ENSURE_OPEN;
	int rc = xmlTextWriterStartPI(DATA(self)->writer, (xmlChar*)target);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_endPI(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	int rc = xmlTextWriterEndPI(DATA(self)->writer);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_writePI(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	const char *target = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 0);
	const char *content = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 1);

	ENSURE_OPEN;
	int rc = xmlTextWriterWritePI(DATA(self)->writer, (xmlChar*)target, (xmlChar*)content);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_startCDATA(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	int rc = xmlTextWriterStartCDATA(DATA(self)->writer);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_endCDATA(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	int rc = xmlTextWriterEndCDATA(DATA(self)->writer);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_writeCDATA(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	const char *content = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 0);

	ENSURE_OPEN;
	int rc = xmlTextWriterWriteCDATA(DATA(self)->writer, (xmlChar*)content);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_startDTD(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	const char *name = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 0);
	const char *pubid = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 1);
	const char *sysid = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 2);

	ENSURE_OPEN;
	int rc = xmlTextWriterStartDTD(DATA(self)->writer, (xmlChar*)name, (xmlChar*)pubid, (xmlChar*)sysid);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_endDTD(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	int rc = xmlTextWriterEndDTD(DATA(self)->writer);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_writeDTD(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	const char *name = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 0);
	const char *pubid = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 1);
	const char *sysid = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 2);
	const char *subset = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 3);

	ENSURE_OPEN;
	int rc = xmlTextWriterWriteDTD(DATA(self)->writer, (xmlChar*)name, (xmlChar*)pubid, (xmlChar*)sysid, (xmlChar*)subset);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_startDTDElement(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	const char *name = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 0);

	ENSURE_OPEN;
	int rc = xmlTextWriterStartDTDElement(DATA(self)->writer, (xmlChar*)name);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_endDTDElement(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	int rc = xmlTextWriterEndDTDElement(DATA(self)->writer);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_writeDTDElement(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	const char *name = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 0);
	const char *content = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 1);

	ENSURE_OPEN;
	int rc = xmlTextWriterWriteDTDElement(DATA(self)->writer, (xmlChar*)name, (xmlChar*)content);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_startDTDAttlist(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	const char *name = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 0);

	ENSURE_OPEN;
	int rc = xmlTextWriterStartDTDAttlist(DATA(self)->writer, (xmlChar*)name);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_endDTDAttlist(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	int rc = xmlTextWriterEndDTDAttlist(DATA(self)->writer);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_writeDTDAttlist(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	const char *name = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 0);
	const char *content = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 1);

	ENSURE_OPEN;
	int rc = xmlTextWriterWriteDTDAttlist(DATA(self)->writer, (xmlChar*)name, (xmlChar*)content);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_writeStartDTDEntity(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	IoObject *peObj = IoMessage_locals_valueArgAt_(m, locals, 0);
	const char *name = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 1);

	IOASSERT(ISBOOL(peObj), "First argument must be true, false or nil.");

	const int pe = ISTRUE(peObj);

	ENSURE_OPEN;
	int rc = xmlTextWriterStartDTDEntity(DATA(self)->writer, pe, (xmlChar*)name);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_writeEndDTDEntity(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	int rc = xmlTextWriterEndDTDEntity(DATA(self)->writer);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_writeDTDInternalEntity(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	IoObject *peObj = IoMessage_locals_valueArgAt_(m, locals, 0);
	const char *name = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 1);
	const char *content = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 2);

	IOASSERT(ISBOOL(peObj), "First argument must be true, false or nil.");

	const int pe = ISTRUE(peObj);

	ENSURE_OPEN;
	int rc = xmlTextWriterWriteDTDInternalEntity(DATA(self)->writer, pe, (xmlChar*)name, (xmlChar*)content);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_writeDTDExternalEntity(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	IoObject *peObj = IoMessage_locals_valueArgAt_(m, locals, 0);
	const char *name = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 1);
	const char *pubid = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 2);
	const char *sysid = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 3);
	const char *ndataid = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 4);

	IOASSERT(ISBOOL(peObj), "First argument must be true, false or nil.");

	const int pe = ISTRUE(peObj);

	ENSURE_OPEN;
	int rc = xmlTextWriterWriteDTDExternalEntity(DATA(self)->writer, pe, (xmlChar*)name, (xmlChar*)pubid, (xmlChar*)sysid, (xmlChar*)ndataid);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_writeDTDExternalEntityContents(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	const char *pubid = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 0);
	const char *sysid = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 1);
	const char *ndataid = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 2);

	ENSURE_OPEN;
	int rc = xmlTextWriterWriteDTDExternalEntityContents(DATA(self)->writer, (xmlChar*)pubid, (xmlChar*)sysid, (xmlChar*)ndataid);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_writeDTDEntity(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	IoObject *peObj = IoMessage_locals_valueArgAt_(m, locals, 0);
	const char *name = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 1);
	const char *pubid = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 2);
	const char *sysid = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 3);
	const char *ndataid = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 4);
	const char *content = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 5);

	IOASSERT(ISBOOL(peObj), "First argument must be true, false or nil.");

	const int pe = ISTRUE(peObj);

	ENSURE_OPEN;
	int rc = xmlTextWriterWriteDTDEntity(DATA(self)->writer, pe, (xmlChar*)name, (xmlChar*)pubid, (xmlChar*)sysid, (xmlChar*)ndataid, (xmlChar*)content);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_writeDTDNotation(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	const char *name = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 0);
	const char *pubid = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 1);
	const char *sysid = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 2);

	ENSURE_OPEN;
	int rc = xmlTextWriterWriteDTDNotation(DATA(self)->writer, (xmlChar*)name, (xmlChar*)pubid, (xmlChar*)sysid);
	IoXmlWriter_raiseErrors_(self, m, rc);

	return self;
}

IoObject *IoXmlWriter_setIndent(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	IoObject *indentObj = IoMessage_locals_valueArgAt_(m, locals, 0);

	IOASSERT(ISBOOL(indentObj), "First argument must be true, false or nil.");

	const int indent = ISTRUE(indentObj);

	ENSURE_OPEN;
	int rc = xmlTextWriterSetIndent(DATA(self)->writer, indent);
	IoXmlWriter_raiseErrors_(self, m, rc);
	return self;
}

IoObject *IoXmlWriter_setIndentString(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	const char *str = IoMessage_locals_cStringArgAtOrNULL_(m, locals, 0);

	ENSURE_OPEN;
	int rc = xmlTextWriterSetIndentString(DATA(self)->writer, (xmlChar*)str);
	IoXmlWriter_raiseErrors_(self, m, rc);
	return self;
}

IoObject *IoXmlWriter_flush(IoXmlWriter *self, IoObject *locals, IoMessage *m)
{
	ENSURE_OPEN;
	int rc = xmlTextWriterFlush(DATA(self)->writer);
	IoXmlWriter_raiseErrors_(self, m, rc);
	return self;
}

