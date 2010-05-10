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
 * @defgroup DOMDocument Document
 * @ingroup DOMNode
 *
 * Document node manipulation.
 *
 * @{
 */
#ifndef _LIBSGML_DOMDOCUMENT_H
#define _LIBSGML_DOMDOCUMENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "DomNode.h"

/**
 * @def DOM_DOCUMENT
 *
 * Defines a DOM_DOCUMENT as a DOM_NODE.  This gives us "inheritance" in C.
 */
#define DOM_DOCUMENT DOM_NODE

/**
 * Constructs an empty DOM document.
 *
 * @return An empty DOM document node.
 */
DOM_DOCUMENT *domDocumentNew();
/**
 * Recursively destroys a DOM document.
 *
 * @param  doc [in] The document to be destroyed.
 */
void domDocumentDestroy(DOM_DOCUMENT *doc);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
