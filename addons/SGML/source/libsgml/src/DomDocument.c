#include <stdlib.h>
#include <stdio.h>

#include <string.h>

#include "DomDocument.h"

DOM_DOCUMENT *domDocumentNew()
{
	DOM_DOCUMENT *ret = domNodeNew(DOM_NODE_TYPE_DOCUMENT, "#document", NULL);

	return ret;
}

void domDocumentDestroy(DOM_DOCUMENT *doc)
{
	if (doc)
		domNodeDestroy(doc);
}
