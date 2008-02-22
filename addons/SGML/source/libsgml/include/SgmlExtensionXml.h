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

#include "SGMLApi.h"

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

SGML_API SGML_EXTENSION_XML *sgmlExtensionXmlNew(SGML_PARSER *parser);
SGML_API void sgmlExtensionXmlDestroy(SGML_PARSER *parser, SGML_EXTENSION_XML *ext);

SGML_API void sgmlExtensionXmlInitialize(SGML_PARSER *parser, void *userContext);
SGML_API void sgmlExtensionXmlElementBegin(SGML_PARSER *parser, void *userContext, const char *elementName);
SGML_API void sgmlExtensionXmlElementEnd(SGML_PARSER *parser, void *userContext, const char *elementName);
SGML_API void sgmlExtensionXmlAttributeNew(SGML_PARSER *parser, void *userContext, const char *attributeName, const char *attributeValue);
SGML_API void sgmlExtensionXmlTextNew(SGML_PARSER *parser, void *userContext, const char *text);
SGML_API void sgmlExtensionXmlCommentNew(SGML_PARSER *parser, void *userContext, const char *comment);
SGML_API void sgmlExtensionXmlDeinitialize(SGML_PARSER *parser, void *userContext);

/**
 * @addtogroup SGMLExtensionXml
 * 
 * @{
 */

/**
 * Currently no parameters are supported for the XML extension.
 */
SGML_API void sgmlExtensionXmlSetParam(SGML_PARSER *parser, void *extensionContext, unsigned long param, void *value);
/**
 * Currently no parameters are supported for the XML extension.
 */
SGML_API void sgmlExtensionXmlGetParam(SGML_PARSER *parser, void *extensionContext, unsigned long param, void *value);

/**
 * @return The 'root' document node.
 */
SGML_API DOM_DOCUMENT *sgmlExtensionXmlGetDocument(SGML_PARSER *parser);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif 

#endif
