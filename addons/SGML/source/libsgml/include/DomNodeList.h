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
 * @defgroup DOMNodeList Node List
 * @ingroup DOM
 *
 * DOM Node list container.
 *
 * @{
 */
#ifndef _LIBSGML_DOMNODELIST_H
#define _LIBSGML_DOMNODELIST_H

#ifdef __cplusplus
extern "C" {
#endif 

#include "DomNode.h"

/**
 * DOM Node List 'node'
 */
typedef struct _dom_node_list_node {

	/**
	 * The DOM_NODE at this position in the list.
	 */
	DOM_NODE                   *node;

	/**
	 * The next entry in the list.
	 */
	struct _dom_node_list_node *next;

} DOM_NODE_LIST_NODE;

/**
 * List of DOM Node List 'nodes'
 */
typedef struct _dom_node_list {

	/**
	 * The first node in the list.
	 */
	struct _dom_node_list_node *firstNode;
	/**
	 * The last node in the list.
	 */
	struct _dom_node_list_node *lastNode;

	/**
	 * The number of entries in the list.
	 */
	unsigned long               entries;

} DOM_NODE_LIST;

/**
 * Constructs an empty node list.
 *
 * @return On success, an empty node list is returned.  Otherwise, NULL is returned.
 */
DOM_NODE_LIST *domNodeListNew();
/**
 * Destroys a node list.
 *
 * @param  nodeList [in] The node list to be destroyed.
 */
void domNodeListDestroy(DOM_NODE_LIST *nodeList);

/**
 * Adds a node to a node list.
 *
 * @param  nodeList [in] The node list context.
 * @param  node     [in] The node to be added.
 */
void domNodeListAddNode(DOM_NODE_LIST *nodeList, DOM_NODE *node);
/**
 * Removes a node from a node list.
 *
 * @param  nodeList [in] The node list context.
 * @param  node     [in] The node to be removed.
 */
void domNodeListRemoveNode(DOM_NODE_LIST *nodeList, DOM_NODE *node);

/**
 * Get the number of nodes in the list.
 *
 * @param  nodeList [in] The node list context.
 * @return The number of nodes in the list.
 */
unsigned long domNodeListGetNumEntries(DOM_NODE_LIST *nodeList);
/**
 * Get the first node list node in the node list.
 *
 * @param  nodeList [in] The node list context.
 * @return A valid node list node will be returned if any nodes are in the list.  Otherwise, NULL is returned.
 */
DOM_NODE_LIST_NODE *domNodeListGetFirstNode(DOM_NODE_LIST *nodeList);

#ifdef __cplusplus
}
#endif 

#endif
