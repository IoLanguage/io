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
 * @defgroup DOMText Text
 * @ingroup DOMNode
 *
 * DOM Text manipulation.
 *
 * @{
 */
#ifndef _LIBSGML_DOMTEXT_H
#define _LIBSGML_DOMTEXT_H

#include "DomNode.h"

/**
 * @def DOM_TEXT
 *
 * Defines DOM_TEXT to DOM_NODE.  This gives us "inheritance" in C.
 */
#define DOM_TEXT DOM_NODE

/**
 * Constructs a new DOM text node with the provided text.
 *
 * @param  text [in] The null terminated string to set the text node to.
 * @return On success, an initialized DOM text node will be returned.  Otherwise, NULL will be returned.
 */
DOM_TEXT *domTextNew(const char *text);

/**
 * @}
 */

#endif
