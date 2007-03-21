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
 * @defgroup DOMElement Element
 * @ingroup  DOMNode
 *
 * Element node manipulation.
 *
 * @{
 */
/**
 * @example variant.c
 * Shows how to manipulate variant element attribute types.
 */
#ifndef _LIBSGML_DOMELEMENT_H
#define _LIBSGML_DOMELEMENT_H

#ifdef __cplusplus
extern "C" {
#endif 

#include "DomNode.h"

/**
 * @def DOM_ELEMENT
 *
 * Defines a DOM_ELEMENT as a DOM_NODE.  This gives us "inheritance" in C.
 */
#define DOM_ELEMENT DOM_NODE

/**
 * Constructs a DOM element with the given name.
 *
 * @param  name [in] The name of the element to construct.
 * @return The initialize DOM element node.
 */
DOM_ELEMENT *domElementNew(const char *name);
/**
 * Recursively destroys a DOM element.
 *
 * @param  element [in] The element to be destroyed.
 */
void domElementDestroy(DOM_ELEMENT *element);

/**
 * Sets an attribute on an element by its name.
 *
 * @param  element [in] The element node to set the attribute on.
 * @param  name    [in] The name of the attribute.
 * @param  value   [in] The value of the attribute.
 */
void domElementSetAttribute(DOM_ELEMENT *element, const char *name, const char *value);
/**
 * Sets an attribute on an element by its name.
 *
 * @ref Variant (For information on variant manipulation)
 *
 * @param  element   [in] The element node to set the attribute on.
 * @param  name      [in] The name of the attribute.
 * @param  type      [in] The type used to interpret the value parameter.
 * @param  value     [in] A pointer to the raw value data.
 * @param  valueSize [in] The size, in bytes, of the value buffer.
 */
void domElementSetAttributeVariant(DOM_ELEMENT *element, const char *name, enum VariantType type, void *value, unsigned long valueSize);

/**
 * Get the value associated with an attribute by its name.
 *
 * @param  element [in] The element node to set the attribute on.
 * @param  name    [in] The name of the attribute.
 * @return On success, a valid pointer is return that is the value associated with the attribute 'name'.  Otherwise, NULL is returned.
 */
const char *domElementGetAttribute(DOM_ELEMENT *element, const char *name);
/**
 * Returns the variant value associated with a given attribute.
 *
 * @ref Variant (For information on variant manipulation)
 *
 * @param  element [in] The node to get the value of.
 * @param  name    [in] The name of the attribute.
 * @param  type    [in] The type used to interpret the value.
 * @return The return value is dependant upon the type argument.
 */
void *domElementGetAttributeVariant(DOM_ELEMENT *element, const char *name, enum VariantType type);

/**
 * Unsets an attribute on an element by its name.
 *
 * @param  element [in] The element node to unset the attribute on.
 * @param  name    [in] The name of the attribute.
 */
void domElementUnsetAttribute(DOM_ELEMENT *element, const char *name);

/**
 * Enumerate the attributes on a given element.
 *
 * @param  element [in] The element to enumerate the attributes of.
 * @param  index   [in] The index into the enumeration.
 * @return If a valid attribute is found at the given index, the name of the attribute is returned.  Otherwise, NULL is returned.
 */
const char *domElementEnumAttributes(DOM_ELEMENT *element, unsigned long index);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif 

#endif
