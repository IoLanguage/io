/*
 * libsgml -- SGML state machine parsing library.
 *                                                                  
 * Copyright (c) 2002 Uninformed Research (http://www.uninformed.org)
 * All rights reserved.
 *
 * skape
 * mmiller@hick.org
 */
/**
 * @defgroup SGMLExtensionXml XML Extension
 * @ingroup SGMLParser
 */
/**
 * @example testxml.c
 * Example code for using the XML extension.
 */
#ifndef _LIBSGML_SGMLEXTENSIONXML_H
#define _LIBSGML_SGMLEXTENSIONXML_H

#ifdef __cplusplus
extern "C" {
#endif 

#include "DomDocument.h"
#include "DomElement.h"
#include "DomText.h"
#include "DomComment.h"

typedef struct _sgml_extension_xml {

	DOM_DOCUMENT *document;
	DOM_ELEMENT  *currElement;

} SGML_EXTENSION_XML;

SGML_EXTENSION_XML *sgmlExtensionXmlNew(SGML_PARSER *parser);
void sgmlExtensionXmlDestroy(SGML_PARSER *parser, SGML_EXTENSION_XML *ext);

void sgmlExtensionXmlInitialize(SGML_PARSER *parser, void *userContext);
void sgmlExtensionXmlElementBegin(SGML_PARSER *parser, void *userContext, const char *elementName);
void sgmlExtensionXmlElementEnd(SGML_PARSER *parser, void *userContext, const char *elementName);
void sgmlExtensionXmlAttributeNew(SGML_PARSER *parser, void *userContext, const char *attributeName, const char *attributeValue);
void sgmlExtensionXmlTextNew(SGML_PARSER *parser, void *userContext, const char *text);
void sgmlExtensionXmlCommentNew(SGML_PARSER *parser, void *userContext, const char *comment);
void sgmlExtensionXmlDeinitialize(SGML_PARSER *parser, void *userContext);

/**
 * @addtogroup SGMLExtensionXml
 * 
 * @{
 */

/**
 * Currently no parameters are supported for the XML extension.
 */
void sgmlExtensionXmlSetParam(SGML_PARSER *parser, void *extensionContext, unsigned long param, void *value);
/**
 * Currently no parameters are supported for the XML extension.
 */
void sgmlExtensionXmlGetParam(SGML_PARSER *parser, void *extensionContext, unsigned long param, void *value);

/**
 * @return The 'root' document node.
 */
DOM_DOCUMENT *sgmlExtensionXmlGetDocument(SGML_PARSER *parser);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif 

#endif
