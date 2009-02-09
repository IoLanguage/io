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
 * @defgroup DOMNode Node
 * @ingroup DOM
 *
 * Core DOM node methods.
 *
 * @{
 */
#ifndef _LIBSGML_DOMNODE_H
#define _LIBSGML_DOMNODE_H

#include "Variant.h"

#ifdef __cplusplus
extern "C" {
#endif 

#include <stdio.h>

#define DOM_NDOE_TYPE_INVALID   0x0000
#define DOM_NODE_TYPE_DOCUMENT  0x0001
#define DOM_NODE_TYPE_ELEMENT   0x0002
#define DOM_NODE_TYPE_ATTRIBUTE 0x0003
#define DOM_NODE_TYPE_TEXT      0x0004
#define DOM_NODE_TYPE_COMMENT   0x0005

struct _dom_node_list;

/**
 *	The most basic form of all DOM nodes.
 */
typedef struct _dom_node {

	/**
	 * The type of node which can be any of the following:
	 *
	 * @li DOM_NODE_TYPE_INVALID <br>
	 * 		The node is not valid.
	 * @li DOM_NODE_TYPE_DOCUMENT <br>
	 * 		The node is a document node.
	 * @li DOM_NODE_TYPE_ELEMENT <br>
	 * 		The node is an element node.
	 * @li DOM_NODE_TYPE_ATTRIBUTE <br>
	 * 		The node is an attribute node.
	 * @li DOM_NODE_TYPE_TEXT <br>
	 * 		The node is a text node.
	 * @li DOM_NODE_TYPE_COMMENT <br>
	 * 		The node is a comment node.
	 */
	unsigned long    type;

	/**
	 * The arbitrary name of the node.
	 */
	char             *name;
	/**
	 * The arbitrary value of the node.
	 */
	char             *value;

	/**
	 * The node's attributes.  Only valid for elements.
	 */
	struct _dom_node *attributes;

	/**
	 * The parent of the node.
	 */
	struct _dom_node *parent;

	/**
	 * The first child of the node.
	 */
	struct _dom_node *firstChild;
	/**
	 * The last child of the node.
	 */
	struct _dom_node *lastChild;

	/**
	 * The previous sibling relative to the node.
	 */
	struct _dom_node *prevSibling;
	/**
	 * The next sibling relative to the node.
	 */
	struct _dom_node *nextSibling;

	/**
	 * Whether or not the node is autoclosing.  Only relative to HTML.
	 */
	unsigned char    autoclose;
	/**
	 * Whether or not the node has deferredClosure.  Only relative to HTML.
	 */
	unsigned char    deferredClosure;
	/**
	 * Whether or not to escape the tags associated with the node.
	 */
	unsigned char    escapeTags;

} DOM_NODE;

/**
 * Constructs a basic node from the provided parameters.
 *
 * Type can be any one of the following:
 *
 * @li DOM_NODE_TYPE_INVALID <br>
 * 		The node is not valid.
 * @li DOM_NODE_TYPE_DOCUMENT <br>
 * 		The node is a document node.
 * @li DOM_NODE_TYPE_ELEMENT <br>
 * 		The node is an element node.
 * @li DOM_NODE_TYPE_ATTRIBUTE <br>
 * 		The node is an attribute node.
 * @li DOM_NODE_TYPE_TEXT <br>
 * 		The node is a text node.
 * @li DOM_NODE_TYPE_COMMENT <br>
 * 		The node is a comment node.
 *
 * @param  type  [in] The type of node.
 * @param  name  [in] The arbitrary name of the node.
 * @param  value [in] THe arbitrary value of the node.
 * @return On success, a pointer to an initialized DOM node is returned.  Otherwise, NULL is returned.
 */
DOM_NODE *domNodeNew(unsigned long type, const char *name, const char *value);
/**
 * Destroys a specific node.  This is only used internally.
 *
 * @param  node [in] The node to deinitialize.
 */
void domNodeDestroySpecific(DOM_NODE *node);
/**
 * Recursively destroys a given node.
 *
 * @param  node [in] The node to destroy.
 */
void domNodeDestroy(DOM_NODE *node);

/**
 * Appends a child to a node.
 *
 * @param  parent [in] The parent that will have a child appended to it.
 * @param  child  [in] The child node to append.
 */
void domNodeAppendChild(DOM_NODE *parent, DOM_NODE *child);
/**
 * Appends a sibling to a node.
 *
 * @param  node    [in] The node to have a sibling appended to it.
 * @param  sibling [in] The sibling to append.
 */
void domNodeAppendSibling(DOM_NODE *node, DOM_NODE *sibling);
/**
 * Removes a child.
 *
 * @param  parent [in] The parent to have the child removed from.
 * @param  child  [in] The child node that is to be removed.
 */
void domNodeRemoveChild(DOM_NODE *parent, DOM_NODE *child);

/**
 * Returns the first child of a node.
 *
 * @param  node [in] The node to get the first child of.
 * @return If the node has a first child, a valid node will be returned.  Otherwise, NULL is returned.
 */
DOM_NODE *domNodeGetFirstChild(DOM_NODE *node);
/**
 * Returns the previous sibling relative to the given node.
 *
 * @param  node [in] The node to get the previous sibling of.
 * @return If the node has a previous sibling, a vaild node will be returned.  Otherwise, NULL is returned.
 */
DOM_NODE *domNodeGetPreviousSibling(DOM_NODE *node);
/**
 * Returns the next sibling relative to the given node.
 *
 * @param  node [in] The node to get the next sibling of.
 * @return If the node has a next sibling, a valid node will be returned.  Otherwise, NULL is returned.
 */
DOM_NODE *domNodeGetNextSibling(DOM_NODE *node);

/**
 * Sets the name of the node.
 *
 * @param  node [in] The node to have the name set on.
 * @param  name [in] The name to set the node to.
 */
void domNodeSetName(DOM_NODE *node, const char *name);
/**
 * Returns the name associated with the node.
 *
 * @param  node [in] The node to get the name of.
 * @return If the node has a name, a valid null terminated string is returned.  Otherwise, NULL is returned.
 */
const char *domNodeGetName(DOM_NODE *node);

/**
 * Sets the string value of the node.
 *
 * @param  node  [in] The node to have the value set on.
 * @param  value [in] The null-terminated string to set the value to.
 */
void domNodeSetValue(DOM_NODE *node, const char *value);

/**
 * Sets the value of the node from a variant type.
 *
 * @param  node      [in] The node to have the value set on.
 * @param  type      [in] The type used to interpret the value argument.
 * @param  value     [in] A pointer to the raw data.
 * @param  valueSize [in] The size, in bytes, of value.
 */
void domNodeSetValueVariant(DOM_NODE *node, enum VariantType type, void *value, unsigned long valueSize);
/**
 * Returns the string value associated with the node.
 *
 * @ref Variant (For information on variant manipulation)
 *
 * @param  node [in] The node to get the value of.
 * @return If the node has a value, a valid null terminated string is returned.  Otherwise, NULL is returned.
 */
const char *domNodeGetValue(DOM_NODE *node);
/**
 * Returns the variant value associated with the string.
 *
 * @ref Variant (For information on variant manipulation)
 *
 * @param  node [in] The node to get the value of.
 * @param  type [in] The type used to interpret the value.
 * @return The return value is dependant upon the type argument.
 */
void *domNodeGetValueVariant(DOM_NODE *node, enum VariantType type);

/**
 * Finds the first node (recursively) that matches the given name.
 *
 * @param  node [in] The node to search on.
 * @param  name [in] The name of the node being searched for.
 * @return If a node with the same name as the one specified is found, a valid node pointer will be returned.  Otherwise, NULL is returned.
 */
DOM_NODE *domNodeFindNodeByName(DOM_NODE *node, const char *name);
/**
 * Finds all nodes (recursively) that match the gievn name.
 *
 * @param  node [in] The node to search on.
 * @param  name [in] The name of the node being searched for.
 * @return If any nodes are found that match the given name, a valid DOM_NODELIST is returned.  Otherwise, NULL is returned.
 */
struct _dom_node_list *domNodeFindNodesByName(DOM_NODE *node, const char *name);
void domNodeFindNodesByName_r(struct _dom_node_list *nodeList, DOM_NODE *node, const char *name);

/**
 * Serializes a given node to a file.
 *
 * @param  node     [in] The node to serialize.
 * @param  fileName [in] The destination file name.
 */
void domNodeSerializeToFile(DOM_NODE *node, const char *fileName);
/**
 * Serializes a given node to a file descriptor.
 *
 * @param  node [in] The node to serialize.
 * @param  fd   [in] The destination file descriptor.
 */
void domNodeSerializeToFd(DOM_NODE *node, FILE *fd);
/**
 * Serializes a given node to a string.  This string must be deallocate with free() when it is dnoe being used.
 *
 * @param  node     [in]  The node to serialize.
 * @param  string   [out] A pointer to a pointer that will hold the null terminated SGML string.
 * @return On success, a value greater than zero is returned that represents the length of the SGML string.  Otherwise, zero is returned.
 */
unsigned long domNodeSerializeToString(DOM_NODE *node, char **string);

void domNodeSerializeToString_r(DOM_NODE *node, char **string, unsigned long *stringLength);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif 

#endif
