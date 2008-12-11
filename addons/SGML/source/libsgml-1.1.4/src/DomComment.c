/*
 * libsgml -- SGML state machine parsing library.
 *                                                                  
 * Copyright (c) 2002 Uninformed Research (http://www.uninformed.org)
 * All rights reserved.
 *
 * skape
 * mmiller@hick.org
 */
#include <stdlib.h>
#include <stdio.h>

#include "DomNode.h"
#include "DomComment.h"

DOM_COMMENT *domCommentNew(const char *comment)
{
	return domNodeNew(DOM_NODE_TYPE_COMMENT, "#comment", comment);
}
