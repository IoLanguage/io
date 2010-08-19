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

#include <string.h>

#include "DomNode.h"
#include "DomNodeList.h"

#if defined(_MSC_VER)
#define strcasecmp strcmpi
#endif

DOM_NODE *domNodeNew(unsigned long type, const char *name, const char *value)
{
	DOM_NODE *node = (DOM_NODE *)malloc(sizeof(DOM_NODE));

	memset(node, 0, sizeof(DOM_NODE));

	node->type  = type;

	if (name)
		node->name  = (char *)strdup(name);
	if (value)
		node->value = (char *)strdup(value);

	return node;
}

void domNodeDestroySpecific(DOM_NODE *node)
{
	domNodeDestroy(node->attributes);

	if (node->name)
		free(node->name);
	if (node->value)
		free(node->value);

	domNodeRemoveChild(node->parent, node);

	free(node);
}

void domNodeDestroy(DOM_NODE *node)
{
	DOM_NODE *curr, *next;

	if (!node)
		return;

	curr = node->firstChild;

	while (curr)
	{
		next = curr->nextSibling;

		domNodeDestroy(curr);

		curr = next;
	}

	if (!node->parent)
	{
		for (curr = node->nextSibling;
				curr;
				curr = curr->nextSibling)
			domNodeDestroy(curr);
	}

	domNodeDestroySpecific(node);
}

void domNodeAppendChild(DOM_NODE *parent, DOM_NODE *child)
{
	if (!parent || !child)
		return;

	if (!parent->firstChild)
		parent->firstChild = child;

	if (parent->lastChild)
	{
		parent->lastChild->nextSibling = child;
		child->prevSibling             = parent->lastChild;
	}
	else
		child->prevSibling = NULL;
	
	parent->lastChild  = child;
	child->parent      = parent;
	child->nextSibling = NULL;
}

void domNodeRemoveChild(DOM_NODE *parent, DOM_NODE *child)
{
	DOM_NODE *curr, *prev = NULL;

	if (!child)
		return;

	if (parent)
	{
		for (curr = parent->firstChild, prev = NULL;
				curr;
				prev = curr, curr = curr->nextSibling)
		{
			if (curr == child)
				break;
		}
	}
	else
		curr = child;

	if (!curr)
		return;

	if (prev)
		prev->nextSibling  = curr->nextSibling;
	else if (parent)
		parent->firstChild = curr->nextSibling;
		
	if (curr->nextSibling)
		curr->nextSibling->prevSibling = prev;

	if ((parent) && (parent->lastChild == curr))
		parent->lastChild = (curr->nextSibling)?curr->nextSibling:prev;

	curr->parent      = NULL;
	curr->nextSibling = NULL;
	curr->prevSibling = NULL;
}

void domNodeAppendSibling(DOM_NODE *node, DOM_NODE *sibling)
{
	DOM_NODE *prev;

	if (!node || !sibling)
		return;

	if ((node->parent) && (prev = node->parent->lastChild))
	{
		prev->nextSibling    = sibling;
		sibling->prevSibling = prev;
	}
	else
	{
		DOM_NODE *curr;

		for (curr = node, prev = NULL;
				curr;
				prev = curr, curr = curr->nextSibling);

		if (!prev)
			prev = node;

		prev->nextSibling    = sibling;
		sibling->prevSibling = prev;
	}

	if (node->parent)
	{
		if (!node->parent->firstChild)
			node->parent->firstChild = sibling;

		node->parent->lastChild = sibling;
	}	

	sibling->parent      = node->parent;
	sibling->nextSibling = NULL;
}

DOM_NODE *domNodeGetFirstChild(DOM_NODE *node)
{
	return (node)?node->firstChild:NULL;
}

DOM_NODE *domNodeGetPreviousSibling(DOM_NODE *node)
{
	return (node)?node->prevSibling:NULL;
}

DOM_NODE *domNodeGetNextSibling(DOM_NODE *node)
{
	return (node)?node->nextSibling:NULL;
}

void domNodeSetName(DOM_NODE *node, const char *name)
{
	if ((!node) || (!name))
		return;

	if (node->name)
		free(node->name);

	node->name = (char *)strdup(name);
}

const char *domNodeGetName(DOM_NODE *node)
{
	return (node)?node->name:NULL;
}

void domNodeSetValue(DOM_NODE *node, const char *value)
{
	if ((!node) || (!value))
		return;

	if (node->value)
		free(node->value);

	node->value = (char *)strdup(value);
}

void domNodeSetValueVariant(DOM_NODE *node, enum VariantType type, void *value, unsigned long valueSize)
{
	variantSet(type, value, valueSize, node, (void (*)(void *, const char *))domNodeSetValue);
}

const char *domNodeGetValue(DOM_NODE *node)
{
	return (node)?node->value:NULL;
}

void *domNodeGetValueVariant(DOM_NODE *node, enum VariantType type)
{
	return variantGet(type, domNodeGetValue(node));
}

DOM_NODE *domNodeFindNodeByName(DOM_NODE *node, const char *name)
{
	DOM_NODE *ret = NULL, *curr;

	if (!node || !name)
		return NULL;

	if ((node->name) && (!strcasecmp(node->name, name)))
		return node;

	for (curr = node->firstChild;
			curr && !ret;
			curr = curr->nextSibling)
		ret = domNodeFindNodeByName(curr, name);

	if (!node->parent && !node->prevSibling)
	{
		for (curr = node->nextSibling;
				curr && !ret;
				curr = curr->nextSibling)
			ret = domNodeFindNodeByName(curr, name);
	}

	return ret;
}

DOM_NODE_LIST *domNodeFindNodesByName(DOM_NODE *node, const char *name)
{
	DOM_NODE_LIST *nodeList = domNodeListNew();

	domNodeFindNodesByName_r(nodeList, node, name);

	if (domNodeListGetNumEntries(nodeList) == 0)
	{
		domNodeListDestroy(nodeList);
		return NULL;
	}
	else
		return nodeList;
}

void domNodeFindNodesByName_r(DOM_NODE_LIST *nodeList, DOM_NODE *node, const char *name)
{
	DOM_NODE *curr;

	if (!node || !name)
		return;

	if ((node->name) && (!strcasecmp(node->name, name)))
		domNodeListAddNode(nodeList, node);

	for (curr = node->firstChild;
			curr;
			curr = curr->nextSibling)
		domNodeFindNodesByName_r(nodeList, curr, name);

	if (!node->parent && !node->prevSibling)
	{
		for (curr = node->nextSibling;
				curr;
				curr = curr->nextSibling)
			domNodeFindNodesByName_r(nodeList, curr, name);
	}
}

void domNodeSerializeToFile(DOM_NODE *node, const char *fileName)
{
	FILE *fd = fopen(fileName, "w");

	if (fd)
	{
		domNodeSerializeToFd(node, fd);

		fclose(fd);
	}
}

void domNodeSerializeToFd(DOM_NODE *node, FILE *fd)
{
	unsigned char elementClosed = 0;
	DOM_NODE *curr;

	if (!node)
		return;

	switch (node->type)
	{
		case DOM_NODE_TYPE_TEXT:
			if (node->value)
				fprintf(fd, "%s", node->value);
			break;
		case DOM_NODE_TYPE_ELEMENT:		
			if (node->escapeTags)
				fprintf(fd, "&lt;%s", node->name);
			else	
				fprintf(fd, "<%s", node->name);

			for (curr = node->attributes;
					curr;
					curr = curr->nextSibling)
			{
				fprintf(fd, " %s", curr->name);

				if (curr->value && *curr->value)
					fprintf(fd, "=\"%s\"", curr->value);
			}

			if (!node->firstChild)
			{
				if (node->escapeTags)
					fprintf(fd, "/&gt;");
				else
					fprintf(fd, "/>");
				elementClosed = 1;
			}
			else
			{
				if (node->escapeTags)
					fprintf(fd, "&gt;");
				else
					fprintf(fd, ">");
			}
			
			break;
		case DOM_NODE_TYPE_COMMENT:
			if (node->value)
				fprintf(fd, "<!--%s-->", node->value);
			break;
	}

	for (curr = node->firstChild;
			curr;
			curr = curr->nextSibling)
		domNodeSerializeToFd(curr, fd);

	if ((node->type == DOM_NODE_TYPE_ELEMENT) && (!elementClosed) && (!node->autoclose))
	{
		if (node->escapeTags)
			fprintf(fd, "&lt;/%s&gt;", node->name);
		else
			fprintf(fd, "</%s>", node->name);
	}
}

unsigned long domNodeSerializeToString(DOM_NODE *node, char **string)
{
	unsigned long stringLength = 1;

	if (string)
	{
		*string = (char *)malloc(stringLength);

		if (*string)
		{
			*string = 0;

			domNodeSerializeToString_r(node, string, &stringLength);
		}
	}

	return stringLength - 1;
}

void domNodeSerializeToString_r(DOM_NODE *node, char **outString, unsigned long *outStringLength)
{
	DOM_NODE *curr;
	unsigned char elementClosed = 0;
	unsigned long newLength;
	char *newString;

	if (!node)
		return;

	switch (node->type)
	{
		case DOM_NODE_TYPE_TEXT:
			if (node->value && strlen(node->value))
			{
				newLength = *outStringLength + strlen(node->value);
				newString = (char *)realloc(*outString, newLength);

				sprintf(newString + *outStringLength - 1, "%s", node->value);

				*outString       = newString;
				*outStringLength = newLength;
			}
			break;
		case DOM_NODE_TYPE_ELEMENT:		
			if (node->escapeTags)
			{
				newLength = *outStringLength + 4 + strlen(node->name);
				newString = (char *)realloc(*outString, newLength);

				sprintf(newString + *outStringLength - 1, "&lt;%s", node->name);

				*outString       = newString;
				*outStringLength = newLength;
			}
			else	
			{
				newLength = *outStringLength + 1 + strlen(node->name);
				newString = (char *)realloc(*outString, newLength);

				sprintf(newString + *outStringLength - 1, "<%s", node->name);

				*outString       = newString;
				*outStringLength = newLength;
			}

			for (curr = node->attributes;
					curr;
					curr = curr->nextSibling)
			{
				newLength = *outStringLength + 1 + strlen(curr->name);
				newString = (char *)realloc(*outString, newLength);

				sprintf(newString + *outStringLength - 1, " %s", curr->name);

				*outString       = newString;
				*outStringLength = newLength;

				if (curr->value && *curr->value)
				{
					newLength = *outStringLength + 3 + strlen(curr->value);
					newString = (char *)realloc(*outString, newLength);
	
					sprintf(newString + *outStringLength - 1, "=\"%s\"", curr->value);
	
					*outString       = newString;
					*outStringLength = newLength;
				}
			}

			if (!node->firstChild)
			{
				if (node->escapeTags)
				{
					newLength = *outStringLength + 5;
					newString = (char *)realloc(*outString, newLength);
	
					sprintf(newString + *outStringLength - 1, "/&gt;");
	
					*outString       = newString;
					*outStringLength = newLength;
				}
				else
				{
					newLength = *outStringLength + 2;
					newString = (char *)realloc(*outString, newLength);
	
					sprintf(newString + *outStringLength - 1, "/>");
	
					*outString       = newString;
					*outStringLength = newLength;
				}

				elementClosed = 1;
			}
			else
			{
				if (node->escapeTags)
				{
					newLength = *outStringLength + 4;
					newString = (char *)realloc(*outString, newLength);
	
					sprintf(newString + *outStringLength - 1, "&gt;");
	
					*outString       = newString;
					*outStringLength = newLength;
				}
				else
				{
					newLength = *outStringLength + 1;
					newString = (char *)realloc(*outString, newLength);
	
					sprintf(newString + *outStringLength - 1, ">");
	
					*outString       = newString;
					*outStringLength = newLength;
				}
			}
			
			break;
		case DOM_NODE_TYPE_COMMENT:
			if (node->value && strlen(node->value))
			{
				newLength = *outStringLength + 7 + strlen(node->value);
				newString = (char *)realloc(*outString, newLength);

				sprintf(newString + *outStringLength - 1, "<!--%s-->", node->value);

				*outString       = newString;
				*outStringLength = newLength;
			}
			break;
	}

	for (curr = node->firstChild;
			curr;
			curr = curr->nextSibling)
		domNodeSerializeToString_r(curr, outString, outStringLength);

	if ((node->type == DOM_NODE_TYPE_ELEMENT) && (!elementClosed) && (!node->autoclose))
	{
		if (node->escapeTags)
		{
			newLength = *outStringLength + 8 + strlen(node->name);
			newString = (char *)realloc(*outString, newLength);

			sprintf(newString + *outStringLength - 1, "&lt;%s&gt;", node->name);

			*outString       = newString;
			*outStringLength = newLength;
		}
		else
		{
			newLength = *outStringLength + 3 + strlen(node->name);
			newString = (char *)realloc(*outString, newLength);

			sprintf(newString + *outStringLength - 1, "</%s>", node->name);

			*outString       = newString;
			*outStringLength = newLength;
		}
	}
}
