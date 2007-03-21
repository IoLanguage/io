/*
docCopyright("Jonathan Wright", 2006)
docLicense("BSD revised")
*/

#ifndef IOXML_WRITER_H
#define IOXML_WRITER_H 1

#include "IoObject.h"
#include "IoSeq.h"
#include <libxml/xmlwriter.h>
#include "IoFile.h"

#define ISXMLWRITER(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoXmlWriter_rawClone)

typedef IoObject IoXmlWriter;

typedef struct
{
    xmlBufferPtr buffer;
	xmlTextWriterPtr writer;
} IoXmlWriterData;

IoXmlWriter *IoXmlWriter_rawClone(IoXmlWriter *self);
IoXmlWriter *IoXmlWriter_proto(void *state);
IoXmlWriter *IoXmlWriter_new(void *state);

void IoXmlWriter_free(IoXmlWriter *self);
void IoXmlWriter_mark(IoXmlWriter *self);

IoObject *IoXmlWriter_openFilename(IoXmlWriter *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlWriter_openStandardOut(IoXmlWriter *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlWriter_openFd(IoXmlWriter *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlWriter_open(IoXmlWriter *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlWriter_content(IoXmlWriter *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlWriter_close(IoXmlWriter *self, IoObject *locals, IoMessage *m);

IoObject *IoXmlWriter_flush(IoXmlWriter *self, IoObject *locals, IoMessage *m);

IoObject *IoXmlWriter_setIndent(IoXmlWriter *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlWriter_setIndentString(IoXmlWriter *self, IoObject *locals, IoMessage *m);

IoObject *IoXmlWriter_startDocument(IoXmlWriter *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlWriter_endDocument(IoXmlWriter *self, IoObject *locals, IoMessage *m);

IoObject *IoXmlWriter_startComment(IoXmlWriter *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlWriter_endComment(IoXmlWriter *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlWriter_writeComment(IoXmlWriter *self, IoObject *locals, IoMessage *m);

IoObject *IoXmlWriter_startElement(IoXmlWriter *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlWriter_startElementNS(IoXmlWriter *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlWriter_endElement(IoXmlWriter *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlWriter_fullEndElement(IoXmlWriter *self, IoObject *locals, IoMessage *m);

IoObject *IoXmlWriter_writeElement(IoXmlWriter *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlWriter_writeElementNS(IoXmlWriter *self, IoObject *locals, IoMessage *m);

IoObject *IoXmlWriter_writeRaw(IoXmlWriter *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlWriter_writeString(IoXmlWriter *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlWriter_writeBase64(IoXmlWriter *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlWriter_writeBinHex(IoXmlWriter *self, IoObject *locals, IoMessage *m);

IoObject *IoXmlWriter_startAttribute(IoXmlWriter *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlWriter_startAttributeNS(IoXmlWriter *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlWriter_endAttribute(IoXmlWriter *self, IoObject *locals, IoMessage *m);

IoObject *IoXmlWriter_writeAttribute(IoXmlWriter *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlWriter_writeAttributeNS(IoXmlWriter *self, IoObject *locals, IoMessage *m);

IoObject *IoXmlWriter_startPI(IoXmlWriter *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlWriter_endPI(IoXmlWriter *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlWriter_writePI(IoXmlWriter *self, IoObject *locals, IoMessage *m);

IoObject *IoXmlWriter_startCDATA(IoXmlWriter *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlWriter_endCDATA(IoXmlWriter *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlWriter_writeCDATA(IoXmlWriter *self, IoObject *locals, IoMessage *m);

IoObject *IoXmlWriter_startDTD(IoXmlWriter *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlWriter_endDTD(IoXmlWriter *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlWriter_writeDTD(IoXmlWriter *self, IoObject *locals, IoMessage *m);

IoObject *IoXmlWriter_startDTDElement(IoXmlWriter *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlWriter_endDTDElement(IoXmlWriter *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlWriter_writeDTDElement(IoXmlWriter *self, IoObject *locals, IoMessage *m);

IoObject *IoXmlWriter_startDTDAttlist(IoXmlWriter *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlWriter_endDTDAttlist(IoXmlWriter *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlWriter_writeDTDAttlist(IoXmlWriter *self, IoObject *locals, IoMessage *m);

IoObject *IoXmlWriter_writeStartDTDEntity(IoXmlWriter *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlWriter_writeEndDTDEntity(IoXmlWriter *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlWriter_writeDTDInternalEntity(IoXmlWriter *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlWriter_writeDTDExternalEntity(IoXmlWriter *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlWriter_writeDTDExternalEntityContents(IoXmlWriter *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlWriter_writeDTDEntity(IoXmlWriter *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlWriter_writeDTDNotation(IoXmlWriter *self, IoObject *locals, IoMessage *m);

#endif
