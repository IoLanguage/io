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
#include "DomText.h"

DOM_TEXT *domTextNew(const char *text)
{
	return domNodeNew(DOM_NODE_TYPE_TEXT, "#text", text);
}
