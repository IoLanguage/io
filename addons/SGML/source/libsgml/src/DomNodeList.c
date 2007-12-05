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

#include "DomNodeList.h"

DOM_NODE_LIST *domNodeListNew()
{
	DOM_NODE_LIST *nodeList = (DOM_NODE_LIST *)malloc(sizeof(DOM_NODE_LIST));

	memset(nodeList, 0, sizeof(DOM_NODE_LIST));

	return nodeList;
}

void domNodeListDestroy(DOM_NODE_LIST *nodeList)
{
	DOM_NODE_LIST_NODE *curr = NULL;

	if (!nodeList)
		return;

	while (nodeList->firstNode)
	{
		curr = nodeList->firstNode->next;

		free(nodeList->firstNode);

		nodeList->firstNode = curr;

		nodeList->entries--;
	}
}

void domNodeListAddNode(DOM_NODE_LIST *nodeList, DOM_NODE *node)
{
	DOM_NODE_LIST_NODE *new;

	if (!nodeList)
		return;

 	new       = (DOM_NODE_LIST_NODE *)malloc(sizeof(DOM_NODE_LIST_NODE));
	new->node = node;
	new->next = NULL;

	if (!nodeList->firstNode)
		nodeList->firstNode      = new;
	if (!nodeList->lastNode)
		nodeList->lastNode       = new;
	else
		nodeList->lastNode->next = new;

	nodeList->lastNode = new;
	
	nodeList->entries++;
}

void domNodeListRemoveNode(DOM_NODE_LIST *nodeList, DOM_NODE *node)
{
	DOM_NODE_LIST_NODE *curr, *prev = NULL;

	if (!nodeList)
		return;

	for (curr = nodeList->firstNode;
			curr;
			curr = curr->next)
	{
		prev = curr;

		if (curr->node == node)
			break;
	}

	if (!curr)
		return;

	if (!prev)
		nodeList->firstNode = curr->next;
	else
		prev->next          = curr->next;

	if (!nodeList->firstNode)
		nodeList->lastNode  = NULL;
	else if (nodeList->lastNode == curr)
		nodeList->lastNode  = prev;

	free(curr);

	nodeList->entries--;
}

unsigned long domNodeListGetNumEntries(DOM_NODE_LIST *nodeList)
{
	return (nodeList)?nodeList->entries:0;
}

DOM_NODE_LIST_NODE *domNodeListGetFirstNode(DOM_NODE_LIST *nodeList)
{
	return (nodeList)?nodeList->firstNode:NULL;
}
