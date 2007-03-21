/*
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
*/

#ifdef IOMESSAGE_C 
#define IO_IN_C_FILE
#endif
#include "Common_inline.h"
#ifdef IO_DECLARE_INLINES

/*
#include "IoObject.h"

#define IOMESSAGEDATA(self) ((IoMessageData *)IoObject_dataPointer(self))

IOINLINE IoSymbol *IoMessage_name(IoMessage *self)
{ 
    return IOMESSAGEDATA(self)->name; 
}

IOINLINE void IoMessage_addArg_(IoMessage *self, IoMessage *m)
{ 
    List_append_(IOMESSAGEDATA(self)->args, IOREF(m)); 
}

IOINLINE IoMessage *IoMessage_rawArgAt_(IoMessage *self, int n)
{ 
    return List_at_(IOMESSAGEDATA(self)->args, n); 
}

IOINLINE IoSymbol *IoMessage_rawLabel(IoMessage *self)
{ 
    return IOMESSAGEDATA(self)->label; 
}

IOINLINE List *IoMessage_rawArgs(IoMessage *self)
{ 
    return IOMESSAGEDATA(self)->args; 
}

IOINLINE IoObject *IoMessage_name(self)  
{
    return (((IoMessageData *)IoObject_dataPointer(self))->name);
}

*/

#undef IO_IN_C_FILE
#endif
