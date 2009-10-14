
//metadoc XmlReader copyright Brian Luczkiewicz, 2006
//metadoc XmlReader license BSD revised

#ifndef IOXML_READER_H
#define IOXML_READER_H 1

#include "IoObject.h"
#include "IoSeq.h"
#include <libxml/xmlreader.h>
#include "IoFile.h"

#define ISXMLREADER(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoXmlReader_rawClone)

typedef IoObject IoXmlReader;

typedef struct
{
	xmlTextReaderPtr reader;
	int options;
	IoSymbol *encoding;
	IoSymbol *url;
	char *error;

	IoSeq *xmlText;
	IoFile *xmlFile;
	IoSymbol *xmlPath;
} IoXmlReaderData;

IoXmlReader *IoXmlReader_rawClone(IoXmlReader *self);
IoXmlReader *IoXmlReader_proto(void *state);
IoXmlReader *IoXmlReader_new(void *state);

void IoXmlReader_free(IoXmlReader *self);
void IoXmlReader_mark(IoXmlReader *self);

void IoXmlReader_showError(IoXmlReader *self);
const char *IoXmlReader_error(IoXmlReader *self);

IoObject *IoXmlReader_test(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_parseString(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_parseFile(IoXmlReader *self, IoObject *locals, IoMessage *m);

IoObject *IoXmlReader_setUrl(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_url(IoXmlReader *self, IoObject *locals, IoMessage *m);

IoObject *IoXmlReader_setEncoding(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_encoding(IoXmlReader *self, IoObject *locals, IoMessage *m);

IoObject *IoXmlReader_xmlFile(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_xmlText(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_xmlPath(IoXmlReader *self, IoObject *locals, IoMessage *m);

// xmlParserProperties
IoObject *IoXmlReader_setLoadDtd(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_loadDtd(IoXmlReader *self, IoObject *locals, IoMessage *m);

IoObject *IoXmlReader_setDefaultAttrs(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_defaultAttrs(IoXmlReader *self, IoObject *locals, IoMessage *m);

IoObject *IoXmlReader_setValidate(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_validate(IoXmlReader *self, IoObject *locals, IoMessage *m);

IoObject *IoXmlReader_setSubstEntities(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_substEntities(IoXmlReader *self, IoObject *locals, IoMessage *m);

// methods
IoObject *IoXmlReader_attributeCount(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_baseUri(IoXmlReader *self, IoObject *locals, IoMessage *m);
//IoObject *IoXmlReader_byteConsumed(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_close(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_localName(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_name(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_namespaceUri(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_value(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_xmlLang(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_xmlVersion(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_depth(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_getAttribute(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_getAttributeNo(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_getAttributeNs(IoXmlReader *self, IoObject *locals, IoMessage *m);
//IoObject *IoXmlReader_columnNumber(IoXmlReader *self, IoObject *locals, IoMessage *m);
//IoObject *IoXmlReader_lineNumber(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_hasValue(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_hasAttributes(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_isDefault(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_isEmptyElement(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_isNamespaceDecl(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_isValid(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_lookupNamespace(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_moveToAttribute(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_moveToAttributeNo(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_moveToAttributeNs(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_moveToElement(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_moveToFirstAttribute(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_moveToNextAttribute(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_next(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_nextSibling(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_nodeType(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_prefix(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_quoteChar(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_read(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_readAttributeValue(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_readInnerXml(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_readOuterXml(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_readState(IoXmlReader *self, IoObject *locals, IoMessage *m);
IoObject *IoXmlReader_readString(IoXmlReader *self, IoObject *locals, IoMessage *m);

#endif
