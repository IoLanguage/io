// metadoc Block copyright Steve Dekorte 2002
// metadoc Block license BSD revised

#ifndef IOBLOCK_DEFINED
#define IOBLOCK_DEFINED 1

#include "Common.h"
#include "List.h"
#include "IoObject.h"
#include "IoState.h"
#include "IoCall.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ISBLOCK(self)                                                          \
    IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoBlock_rawClone)

typedef IoObject IoBlock;

typedef struct {
    IoMessage *message;
    List *argNames;
    IoObject *scope; // if 0x0, then use target as the locals proto
    int passStops;
    clock_t profilerTime;
} IoBlockData;

IoBlock *IoBlock_proto(void *state);
IoBlock *IoBlock_rawClone(IoBlock *self);
IoBlock *IoBlock_new(IoState *state);
void IoBlock_mark(IoBlock *self);
void IoBlock_free(IoBlock *self);
void IoBlock_message_(IoBlock *self, IoMessage *m);

// void IoBlock_writeToStream_(IoBlock *self, BStream *stream);
// void IoBlock_readFromStream_(IoBlock *self, BStream *stream);

// calling

IOVM_API IoObject *IoBlock_activate(IoBlock *self, IoObject *target,
                                    IoObject *locals, IoMessage *m,
                                    IoObject *slotContext);
IO_METHOD(IoBlock, print);

UArray *IoBlock_justCode(IoBlock *self);
IO_METHOD(IoBlock, code);
IO_METHOD(IoBlock, code_);

IO_METHOD(IoBlock, message);
IO_METHOD(IoBlock, setMessage);

IO_METHOD(IoBlock, argumentNames);
IO_METHOD(IoBlock, argumentNames_);

IO_METHOD(IoBlock, setScope_);
IO_METHOD(IoBlock, scope);

IO_METHOD(IoBlock, setPassStops_);
IO_METHOD(IoBlock, passStops);

IO_METHOD(IoBlock, setProfilerOn);
IO_METHOD(IoBlock, profilerTime);
void IoBlock_rawResetProfilerTime(IoBlock *self);
clock_t IoBlock_rawProfilerTime(IoBlock *self);

// lobby methods

IoObject *IoBlock_method(IoObject *target, IoObject *locals, IoMessage *m);
IoObject *IoObject_block(IoObject *target, IoObject *locals, IoMessage *m);

IO_METHOD(IoBlock, performOn);
IO_METHOD(IoBlock, call);

#ifdef __cplusplus
}
#endif
#endif
