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
 * @defgroup DOMComment Comment
 * @ingroup DOMNode
 *
 * Comment node manipulation.
 * 
 * @{
 */
#ifndef _LIBSGML_DOMCOMMENT_H
#define _LIBSGML_DOMCOMMENT_H

#include "DomNode.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def DOM_COMMENT
 *
 * Defines a DOM_COMMENT as a DOM_NODE.  This gives us "inheritance" in C.
 */
#define DOM_COMMENT DOM_NODE

/**
 * Creates a new DOM comment with the provided comment text.
 *
 * @param  comment [in] The text value for the comment.
 * @return The constructed DOM comment node.
 */
DOM_COMMENT *domCommentNew(const char *comment);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
