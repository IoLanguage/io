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
 * @defgroup Variant Variant type manipulation
 *
 * Variant type manipulation and coersion.
 *
 * @{
 */
/**
 * @example variant.c
 * Shows basic usage of variant manipulation between types.
 */
#ifndef _LIBSGML_VARIANT_H
#define _LIBSGML_VARIANT_H

/**
 * The variable type enumeration.
 */
enum VariantType {

	/**
	 * Unknown type
	 */
	VARIANT_TYPE_UNKNOWN = 0,

	/**
	 * Null terminated string (const char *)
	 */
	VARIANT_TYPE_STRING,
	/**
	 * Multiple strings (array)
	 *
	 * @remarks This is currently unsupported.
	 */
	VARIANT_TYPE_MULTI_STRING,

	/**
	 * Signed integer (signed long)
	 */
	VARIANT_TYPE_SIGNED_INTEGER,
	/**
	 * Unsigned integer (unsigned long)
	 */
	VARIANT_TYPE_UNSIGNED_INTEGER,

	/**
	 * Signed short (signed short)
	 */
	VARIANT_TYPE_SIGNED_SHORT,
	/**
	 * Unsigned short (unsigned short)
	 */
	VARIANT_TYPE_UNSIGNED_SHORT,

	/**
	 * Float (float)
	 *
	 * @remarks This type, when used with variantGet returns a VARIANT_VALUE structure that must be deallocated via variantFree.
	 */
	VARIANT_TYPE_FLOAT,
	/**
	 * Double (double)
	 *
	 * @remarks This type, when used with variantGet returns a VARIANT_VALUE structure that must be deallocated via variantFree.
	 */
	VARIANT_TYPE_DOUBLE,

	/**
	 * Character (char)
	 */
	VARIANT_TYPE_CHAR,

	/**
	 * Binary_B64 (unsigned char *)
	 *
	 * @remarks This type, when used with variantGet returns a VARIANT_VALUE structure that must be deallocated via variantFree.
	 * 			The binary type assumes that the value was originally set as a VARIANT_TYPE_BINARY_B64.  Binary types are non-convertable.  Trying to convert a value that was stored
	 * 			as a binary to any other type is not suggested and may cause unpredictable results.  The reason they are not convertable is because they are stored as
	 * 			a base64 encoded string.
	 */
	VARIANT_TYPE_BINARY_B64

};

/**
 * @brief Variant value structure.
 *
 * This structure is used for returning values from variantGet that do not fit within 4 bytes.
 */
typedef struct _variant_value {

	/**
	 * Filled in when the type is VARIANT_TYPE_FLOAT.
	 */
	float         flt;
	/**
	 * Filled in when the type is VARIANT_TYPE_DOUBLE.
	 */
	double        dbl;
	/**
	 * Filled in when the type is VARIANT_TYPE_BINARY_B64.
	 */
	unsigned char *binary;

	/**
	 * Always filled in with the length.
	 */
	unsigned long length;

} VARIANT_VALUE;

/**
 * Converts the raw data passed in to a string and passes it to the 'setter' callback.
 *
 * @param  type      [in] The variant type that is used to interpret the value argument.
 * @param  value     [in] A pointer to the raw data.
 * @param  valueSize [in] The size of the value buffer in bytes.
 * @param  userParam [in] The user parameter passed in as the first argument to 'setter'.
 * @param  setter    [in] The callback used to do the actual 'setting' of the converted type.
 */
void variantSet(enum VariantType type, void *value, unsigned long valueSize, void *userParam, void (*setter)(void *userParam, const char *string));
/**
 * Returns the variant value associated with the string.
 *
 * The return values expected for each type are:
 *
 * @li VARIANT_TYPE_STRING <br>
 * 		A pointer to a null terminated string.  This buffer should be considered constant and remain unmodified.
 * @li VARIANT_TYPE_SIGNED_INTEGER <br>
 * 		A signed integer is returned.
 * @li VARIANT_TYPE_UNSIGNED_INTEGER <br>
 * 		An unsigned integer is returned.
 * @li VARIANT_TYPE_SIGNED_SHORT <br>
 * 		A signed short is returned.
 * @li VARIANT_TYPE_UNSIGNED_SHORT <br>
 * 		An unsigned short is returned.
 * @li VARIANT_TYPE_FLOAT <br>
 * 		A pointer to a VARIANT_VALUE structure is returned with the 'flt' member populated.  <b>This structure is dynamically allocated and should be free'd with VariantFree</b>.
 * @li VARIANT_TYPE_DOUBLE <br>
 * 		A pointer to a VARIANT_VALUE structure is returned with the 'dbl' member populated.  <b>This structure is dynamically allocated and should be free'd with VariantFree</b>.
 * @li VARIANT_TYPE_CHAR <br>
 * 		A signed character is returned.
 * @li VARIANT_TYPE_BINARY_B64 <br>
 * 		A pointer to a VARIANT_VALUE structure is returned with the 'binary' member populated.  <b>This structure is dynamically allocated and should be free'd with VariantFree</b>.
 *
 * If the operation fails to render the value into a given type, NULL wll be returned.
 *
 * @param  type   [in] The type to interpret the string as.
 * @param  string [in] The null terminated string.
 * @return The return value is arbitrary and dependent on the type.
 */
void *variantGet(enum VariantType type, const char *string);

/**
 * Deallocates the variant value passed in.
 *
 * This is used to clean up memory returned from variantGet for some types.
 *
 * @param  value [in] The variant value to destroy.
 */
void variantFree(VARIANT_VALUE *value);

/**
 * @}
 */

#endif
