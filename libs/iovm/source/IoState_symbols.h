
// metadoc State copyright Steve Dekorte 2002
// metadoc State license BSD revised

IOVM_API void IoState_setupCachedNumbers(IoState *self);
IOVM_API IoObject *IoState_numberWithDouble_(IoState *self, double n);

IOVM_API IoSymbol *IoState_symbolWithUArray_copy_(IoState *self, UArray *ba,
                                                  int copy);
IOVM_API IoSymbol *
IoState_symbolWithUArray_copy_convertToFixedWidth(IoState *self, UArray *ba,
                                                  int copy);
IOVM_API IoSymbol *IoState_symbolWithCString_(IoState *self, const char *s);
IOVM_API IoSymbol *
IoState_symbolWithCString_length_(IoState *self, const char *s, size_t length);

IOVM_API IoSymbol *IoState_addSymbol_(IoState *self, IoSymbol *s);
IOVM_API void IoState_removeSymbol_(IoState *self, IoSymbol *aString);
