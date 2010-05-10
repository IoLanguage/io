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
 * @defgroup SGMLExtensionHtml HTML Extension
 * @ingroup SGMLParser
 */
/**
 * @example testhtml.c
 * Example code for using the HTML extension.
 */
#ifndef _LIBSGML_SGMLEXTENSIONHTML_H
#define _LIBSGML_SGMLEXTENSIONHTML_H

#ifdef __cplusplus
extern "C" {
#endif 

#include "DomDocument.h"
#include "DomElement.h"
#include "DomText.h"
#include "DomComment.h"

#include "DomNodeList.h"

/**
 * @addtogroup SGMLExtensionHtml
 *
 * @{
 */
/**
 * @def SGML_EXTENSION_HTML_PARAM_FLAG
 *
 * HTML extension parameter that allows for the setting of flags.  Flags can be any of the following:
 *
 *	@li SGML_EXTENSION_HTML_FLAG_STRIPELEMENT
 *			Ignores element nodes.
 *	@li SGML_EXTENSION_HTML_FLAG_STRIPTEXT
 *			Ignores text nodes.
 *	@li SGML_EXTENSION_HTML_FLAG_STRIPCOMMENT
 *			Ignores comment nodes.
 *	@li SGML_EXTENSION_HTML_FLAG_IGNOREBLANKLINES
 *			Ignores text nodes that only have whitespace characters.
 *	@li SGML_EXTENSION_HTML_FLAG_ESCAPEUNKNOWNTAGS
 *			Used in conjunction with the SGML_EXTENSION_HTML_PARAM_SETKNOWNTAGS parameter, this allows for escaping unknown element tags.
 */
#define SGML_EXTENSION_HTML_PARAM_FLAG            0x00000001
/**
 * @def SGML_EXTENSION_HTML_PARAM_SETKNOWNTAGS
 *
 * HTML extension parameter for setting 'known tags'.  Known tags are tags that will not be escaped when the document is serialized.
 */
#define SGML_EXTENSION_HTML_PARAM_SETKNOWNTAGS    0x00000002

/**
 * @def SGML_EXTENSION_HTML_FLAG_STRIPELEMENT
 *
 * Used in conjunction with the parameter SGML_EXTENSION_HTML_PARAM_FLAG, this flag specifies that elements are to be ignored during parsing.
 */
#define SGML_EXTENSION_HTML_FLAG_STRIPELEMENT      (1 << 0)
/**
 * @def SGML_EXTENSION_HTML_FLAG_STRIPTEXT
 *
 * Used in conjunction with the parameter SGML_EXTENSION_HTML_PARAM_FLAG, this flag specifies that text nodes are to be ignored during parsing.
 */
#define SGML_EXTENSION_HTML_FLAG_STRIPTEXT         (1 << 1)
/**
 * @def SGML_EXTENSION_HTML_FLAG_STRIPCOMMENT
 *
 * Used in conjunction with the parameter SGML_EXTENSION_HTML_PARAM_FLAG, this flag specifies that comments are to be ignored during parsing.
 */
#define SGML_EXTENSION_HTML_FLAG_STRIPCOMMENT      (1 << 2)
/**
 * @def SGML_EXTENSION_HTML_FLAG_IGNOREBLANKLINES
 *
 * Used in conjunction with the parameter SGML_EXTENSION_HTML_PARAM_FLAG, this flag specifies that text nodes with only white spaces are to be ignored.
 */
#define SGML_EXTENSION_HTML_FLAG_IGNOREBLANKLINES  (1 << 3)
/**
 * @def SGML_EXTENSION_HTML_FLAG_ESCAPEUNKNOWNTAGS
 *
 * Used in conjunction with the parameter SGML_EXTENSION_HTML_PARAM_FLAG, this flag specifies that unknown tags are to be escaped.
 */
#define SGML_EXTENSION_HTML_FLAG_ESCAPEUNKNOWNTAGS (1 << 4)

/**
 * @}
 */

typedef struct _sgml_extension_html {

	DOM_DOCUMENT  *document;
	DOM_ELEMENT   *currElement;

	unsigned char skipNextAttribute;

	unsigned long flags;

	char          **knownTags;

	void (*saveDivert)(struct _sgml_parser *parser, unsigned long newIndex, unsigned long oldState, unsigned long newState, const char *lastBuffer, unsigned long lastBufferSize);	

} SGML_EXTENSION_HTML;

SGML_EXTENSION_HTML *sgmlExtensionHtmlNew(SGML_PARSER *parser);
void sgmlExtensionHtmlDestroy(SGML_PARSER *parser, SGML_EXTENSION_HTML *ext);

void sgmlExtensionHtmlInitialize(SGML_PARSER *parser, void *userContext);
void sgmlExtensionHtmlElementBegin(SGML_PARSER *parser, void *userContext, const char *elementName);
void sgmlExtensionHtmlElementEnd(SGML_PARSER *parser, void *userContext, const char *elementName);
void sgmlExtensionHtmlAttributeNew(SGML_PARSER *parser, void *userContext, const char *attributeName, const char *attributeValue);
void sgmlExtensionHtmlTextNew(SGML_PARSER *parser, void *userContext, const char *text);
void sgmlExtensionHtmlCommentNew(SGML_PARSER *parser, void *userContext, const char *comment);
void sgmlExtensionHtmlDeinitialize(SGML_PARSER *parser, void *userContext);

/**
 * @addtogroup SGMLExtensionHtml
 *
 * @{
 */

/**
 * Sets an arbitrary parameter on the HTML extension.
 *
 * param can be any one of the following:
 *
 * @li SGML_EXTENSION_HTML_PARAM_FLAG
 * 		This is used to set bit vector flags on the extension for use when parsing. 
 *
 * @li SGML_EXTENSION_HTML_PARAM_SETKNOWNTAGS
 * 		This is used to set the tags which will be "known" by the parser as being valid and thus will not be escaped.  This value
 * 		is only used if the 'SGML_EXTENSION_HTML_FLAG_ESCAPEUNKNOWNTAGS flag is set.  The value for this parameter should be an 
 * 		array of null terminated strings, ending with a NULL element.
 *
 * @param  parser           [in] The SGML parser instance.
 * @param  extensionContext [in] The HTML extension context.
 * @param  param            [in] An arbitrary parameter identifier.
 * @param  value            [in] The value to set the parameter to.
 */
void sgmlExtensionHtmlSetParam(SGML_PARSER *parser, void *extensionContext, unsigned long param, void *value);
/**
 * Gets the value of a given parameter on the HTML extension.  See sgmlExtensionHtmlGetParam for a list of parameters.
 *
 * @param  parser           [in]  The SGML parser instance.
 * @param  extensionContext [in]  The HTML extension context.
 * @param  param            [in]  An arbitrary parameter identifier.
 * @param  value            [out] The value to set the parameter to.
 */
void sgmlExtensionHtmlGetParam(SGML_PARSER *parser, void *extensionContext, unsigned long param, void *value);

/**
 * Get the 'root' document node that was parsed in by the parser.
 *
 * @return The document node.
 */
DOM_DOCUMENT *sgmlExtensionHtmlGetDocument(SGML_PARSER *parser);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif 

#endif
