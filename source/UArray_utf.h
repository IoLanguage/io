/*
	copyright: Steve Dekorte, 2006. All rights reserved.
	license: See _BSDLicense.txt.
*/

BASEKIT_API int UArray_convertToFixedSizeType(UArray *self);

BASEKIT_API size_t UArray_numberOfCharacters(const UArray *self); // returns 0 on error

BASEKIT_API int UArray_maxCharSize(const UArray *self);
BASEKIT_API int UArray_isMultibyte(const UArray *self);
BASEKIT_API int UArray_isLegalUTF8(const UArray *self);

BASEKIT_API UArray *UArray_asUTF8(const UArray *self);
BASEKIT_API UArray *UArray_asUCS2(const UArray *self);
BASEKIT_API UArray *UArray_asUCS4(const UArray *self);

BASEKIT_API void UArray_convertToUTF8(UArray *self);
BASEKIT_API void UArray_convertToUCS2(UArray *self);
BASEKIT_API void UArray_convertToUCS4(UArray *self);
