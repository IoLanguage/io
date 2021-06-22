
// metadoc Message copyright Steve Dekorte 2002
// metadoc Message license BSD revised

#ifndef IOMESSAGE_DEFINED
#define IOMESSAGE_DEFINED 1

#include "IoVMApi.h"
#include "Common.h"
#include "List.h"
#include "UArray.h"
#include "IoTag.h"
#include "IoObject_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MESSAGE_STOP_STATUS_NORMAL 0
#define MESSAGE_STOP_STATUS_BREAK 1
#define MESSAGE_STOP_STATUS_CONTINUE 2
#define MESSAGE_STOP_STATUS_RETURN 4
#define MESSAGE_STOP_STATUS_EOL 8

#define ISMESSAGE(self)                                                        \
    IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoMessage_rawClone)

#if !defined(IoSymbol_DEFINED)
#define IoSymbol_DEFINED
typedef IoObject IoSymbol;
#endif

typedef IoObject IoMessage;

#define IOMESSAGE_HASPREV

typedef struct {
    IoSymbol *name;
    List *args;
#ifdef IOMESSAGE_HASPREV
    IoMessage *previous; // unused for now
#endif
    IoMessage *next;
    IoObject *cachedResult;

    // debugging info
    // int charNumber;
    int lineNumber;
    IoSymbol *label;
} IoMessageData;

#define IOMESSAGEDATA(self) ((IoMessageData *)IoObject_dataPointer(self))

// basics

IOVM_API IoMessage *IoMessage_proto(void *state);
IOVM_API IoMessage *IoMessage_rawClone(IoMessage *m);
IOVM_API IoMessage *IoMessage_new(void *state);
IOVM_API void IoMessage_copy_(IoMessage *self, IoMessage *other);
IOVM_API IoMessage *IoMessage_deepCopyOf_(IoMessage *m);
IOVM_API IoMessage *IoMessage_newWithName_(void *state, IoSymbol *symbol);
IOVM_API IoMessage *IoMessage_newWithName_label_(void *state, IoSymbol *symbol,
                                                 IoSymbol *label);
IOVM_API IoMessage *
IoMessage_newWithName_returnsValue_(void *state, IoSymbol *symbol, IoObject *v);
IOVM_API IoMessage *
IoMessage_newWithName_andCachedArg_(void *state, IoSymbol *symbol, IoObject *v);

IOVM_API void IoMessage_mark(IoMessage *self);
IOVM_API void IoMessage_free(IoMessage *self);

IOVM_API IoSymbol *IoMessage_rawLabel(IoMessage *self);
IOVM_API int IoMessage_rawLineNumber(IoMessage *self);
IOVM_API int IoMessage_rawCharNumber(IoMessage *self);
IOVM_API void
IoMessage_label_(IoMessage *self,
                 IoSymbol *ioSymbol); // sets label for children too
IOVM_API void IoMessage_rawSetLineNumber_(IoMessage *self, int n);
IOVM_API void IoMessage_rawSetCharNumber_(IoMessage *self, int n);
IOVM_API void IoMessage_rawCopySourceLocation(IoMessage *self,
                                              IoMessage *other);
IOVM_API List *IoMessage_rawArgList(IoMessage *self);
IOVM_API unsigned char IoMessage_needsEvaluation(IoMessage *self);

IOVM_API void IoMessage_addCachedArg_(IoMessage *self, IoObject *v);
IOVM_API void IoMessage_setCachedArg_to_(IoMessage *self, int n, IoObject *v);
IOVM_API void IoMessage_setCachedArg_toInt_(IoMessage *self, int n, int anInt);
IOVM_API void IoMessage_rawSetCachedResult_(IoMessage *self, IoObject *v);
IOVM_API void IoMessage_rawSetName_(IoMessage *self, IoObject *v);
IOVM_API void IoMessage_rawSetLabel_(IoMessage *self, IoObject *v);

IOVM_API IO_METHOD(IoMessage, lineNumber);
IOVM_API IO_METHOD(IoMessage, characterNumber);
IOVM_API IO_METHOD(IoMessage, label);

// perform

IOVM_API IO_METHOD(IoMessage, doInContext);
IOVM_API IoObject *IoMessage_locals_performOn_(IoMessage *self,
                                               IoObject *locals,
                                               IoObject *target);

// args

// IOVM_API IoObject *IoMessage_locals_valueArgAt_(IoMessage *self, IoObject
// *locals, int n);
IOVM_API IoObject *IoMessage_locals_valueOrEvalArgAt_(IoMessage *self,
                                                      IoObject *locals, int n);
IOVM_API void IoMessage_assertArgCount_receiver_(IoMessage *self, int n,
                                                 IoObject *receiver);
IOVM_API int IoMessage_argCount(IoMessage *self);

IOVM_API void IoMessage_locals_numberArgAt_errorForType_(IoMessage *self,
                                                         IoObject *locals,
                                                         int n,
                                                         const char *typeName);

IOVM_API IoObject *IoMessage_locals_numberArgAt_(IoMessage *self,
                                                 IoObject *locals, int n);
IOVM_API int IoMessage_locals_intArgAt_(IoMessage *self, IoObject *locals,
                                        int n);
IOVM_API int IoMessage_locals_boolArgAt_(IoMessage *self, IoObject *locals,
                                         int n);
IOVM_API long IoMessage_locals_longArgAt_(IoMessage *self, IoObject *locals,
                                          int n);
IOVM_API size_t IoMessage_locals_sizetArgAt_(IoMessage *self, IoObject *locals,
                                             int n);
IOVM_API double IoMessage_locals_doubleArgAt_(IoMessage *self, IoObject *locals,
                                              int n);
IOVM_API float IoMessage_locals_floatArgAt_(IoMessage *self, IoObject *locals,
                                            int n);

IOVM_API IoObject *IoMessage_locals_symbolArgAt_(IoMessage *self,
                                                 IoObject *locals, int n);
IOVM_API IoObject *IoMessage_locals_seqArgAt_(IoMessage *self, IoObject *locals,
                                              int n);
IOVM_API IoObject *IoMessage_locals_mutableSeqArgAt_(IoMessage *self,
                                                     IoObject *locals, int n);
IOVM_API char *IoMessage_locals_cStringArgAt_(IoMessage *self, IoObject *locals,
                                              int n);
IOVM_API IoObject *
IoMessage_locals_valueAsStringArgAt_(IoMessage *self, IoObject *locals, int n);

IOVM_API IoObject *IoMessage_locals_blockArgAt_(IoMessage *self,
                                                IoObject *locals, int n);
IOVM_API IoObject *IoMessage_locals_dateArgAt_(IoMessage *self,
                                               IoObject *locals, int n);
IOVM_API IoObject *IoMessage_locals_mapArgAt_(IoMessage *self, IoObject *locals,
                                              int n);
IOVM_API IoObject *IoMessage_locals_messageArgAt_(IoMessage *self,
                                                  IoObject *locals, int n);
IOVM_API IoObject *IoMessage_locals_listArgAt_(IoMessage *self,
                                               IoObject *locals, int n);

// printing

IOVM_API void IoMessage_print(IoMessage *self);
IOVM_API void IoMessage_printWithReturn(IoMessage *self);
IOVM_API UArray *IoMessage_description(IoMessage *self);
IOVM_API UArray *IoMessage_descriptionJustSelfAndArgs(
    IoMessage *self); /* caller must io_free returned */
IOVM_API void IoMessage_appendDescriptionTo_follow_(IoMessage *self, UArray *ba,
                                                    int follow);
IOVM_API IO_METHOD(IoMessage, asString);

// primitive methods

IOVM_API IO_METHOD(IoMessage, clone);
IOVM_API IO_METHOD(IoMessage, protoName);
IOVM_API IO_METHOD(IoMessage, protoSetName);
IOVM_API IO_METHOD(IoMessage, descriptionString);

// next

IOVM_API IO_METHOD(IoMessage, next);
IOVM_API IoMessage *IoMessage_rawNext(IoMessage *self);

IOVM_API IO_METHOD(IoMessage, setNext);
IOVM_API void IoMessage_rawSetNext_(IoMessage *self, IoMessage *m);

IOVM_API IO_METHOD(IoMessage, isEOL);
IOVM_API int IoMessage_rawIsEOL(IoMessage *self);

IOVM_API IO_METHOD(IoMessage, nextIgnoreEOLs);
IOVM_API IoMessage *IoMessage_rawNextIgnoreEOLs(IoMessage *self);
IOVM_API IO_METHOD(IoMessage, last);
IOVM_API IoMessage *IoMessage_rawLast(IoMessage *self);
IOVM_API IO_METHOD(IoMessage, lastBeforeEOL);
IOVM_API IoMessage *IoMessage_rawLastBeforeEOL(IoMessage *self);

// previous

IOVM_API IO_METHOD(IoMessage, previous);
IOVM_API IoMessage *IoMessage_rawPrevious(IoMessage *self);

IOVM_API IO_METHOD(IoMessage, setPrevious);
IOVM_API void IoMessage_rawSetPrevious(IoMessage *self, IoMessage *m);

IOVM_API IO_METHOD(IoMessage, argAt);
IOVM_API IO_METHOD(IoMessage, arguments);
IOVM_API IO_METHOD(IoMessage, setArguments);
IOVM_API IO_METHOD(IoMessage, appendArg);
IOVM_API IO_METHOD(IoMessage, appendCachedArg);
IOVM_API IO_METHOD(IoMessage, argCount_);
IOVM_API IO_METHOD(IoMessage, cachedResult);
IOVM_API IO_METHOD(IoMessage, setCachedResult);
IOVM_API IO_METHOD(IoMessage, removeCachedResult);
IOVM_API IO_METHOD(IoMessage, hasCachedResult);

IOVM_API IO_METHOD(IoMessage, setLineNumber);
IOVM_API IO_METHOD(IoMessage, setCharacterNumber);
IOVM_API IO_METHOD(IoMessage, setLabel);

IOVM_API IO_METHOD(IoMessage, fromString);
IOVM_API IO_METHOD(IoMessage, argsEvaluatedIn);
IOVM_API IO_METHOD(IoMessage, evaluatedArgs);
IOVM_API IO_METHOD(IoMessage, profilerTime);

IOVM_API void IoMessage_foreachArgs(IoMessage *self, IoObject *object,
                                    IoSymbol **indexSlotName,
                                    IoSymbol **valueSlotName,
                                    IoMessage **doMessage);

IOVM_API IoMessage *IoMessage_asMessageWithEvaluatedArgs(IoMessage *self,
                                                         IoObject *locals,
                                                         IoMessage *m);

//  ------------------------------

#include "IoMessage_inline.h"
#include "IoMessage_parser.h"

// IoSymbol *IoMessage_name(IoMessage *self);

#define IoMessage_name(self)                                                   \
    (((IoMessageData *)IoObject_dataPointer(self))->name)
#define IoMessage_rawCachedResult(self)                                        \
    (((IoMessageData *)IoObject_dataPointer(self))->cachedResult)

IOVM_API void IoMessage_addArg_(IoMessage *self, IoMessage *m);
IOVM_API IoMessage *IoMessage_rawArgAt_(IoMessage *self, int n);
IOVM_API List *IoMessage_rawArgs(IoMessage *self);

IOVM_API UArray *IoMessage_asMinimalStackEntryDescription(IoMessage *self);

#ifdef __cplusplus
}
#endif
#endif
