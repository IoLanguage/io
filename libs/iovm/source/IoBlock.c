// metadoc Block copyright Steve Dekorte 2002
// metadoc Block license BSD revised
/*metadoc Block description
Blocks are anonymous functions (messages with their own locals object).
They are typically used to represent object methods.
*/
// metadoc Block category Core

#include "IoBlock.h"
#include "IoMessage.h"
#include "IoMessage_parser.h"
#include "IoCFunction.h"
#include "IoSeq.h"
#include "IoNumber.h"
#include "IoList.h"
#include "UArray.h"

static const char *protoId = "Block";

#define DATA(self) ((IoBlockData *)IoObject_dataPointer(self))

IoTag *IoBlock_newTag(void *state) {
    IoTag *tag = IoTag_newWithName_(protoId);
    IoTag_state_(tag, state);
    IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoBlock_rawClone);
    IoTag_markFunc_(tag, (IoTagMarkFunc *)IoBlock_mark);
    IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoBlock_free);
    IoTag_activateFunc_(tag, (IoTagActivateFunc *)IoBlock_activate);
    // IoTag_writeToStreamFunc_(tag, (IoTagWriteToStreamFunc
    // *)IoBlock_writeToStream_); IoTag_readFromStreamFunc_(tag,
    // (IoTagReadFromStreamFunc *)IoBlock_readFromStream_);
    return tag;
}

void IoBlock_copy_(IoBlock *self, IoBlock *other) {
    DATA(self)->message = IOREF(DATA(other)->message);

    {
        List *l1 = DATA(self)->argNames;
        List_removeAll(l1);
        LIST_FOREACH(DATA(other)->argNames, i, v, List_append_(l1, IOREF(v)););
    }

    if (DATA(other)->scope) {
        IOREF(DATA(other)->scope);
    }

    DATA(self)->scope = DATA(other)->scope;
}

/*
void IoBlock_writeToStream_(IoBlock *self, BStream *stream)
{
        UArray *ba = IoBlock_justCode(self);
        BStream_writeTaggedUArray_(stream, ba);
        //printf("write block '%s'\n", UArray_asCString(ba));
        UArray_free(ba);

        if (DATA(self)->scope)
        {
                BStream_writeTaggedInt32_(stream,
IoObject_pid(DATA(self)->scope));
        }
        else
        {
                BStream_writeTaggedInt32_(stream, 0);
        }
}

void IoBlock_readFromStream_(IoBlock *self, BStream *stream)
{
        IoBlock *newBlock = NULL;
        UArray *ba = BStream_readTaggedUArray(stream);

        //printf("read block [[[%s]]]]\n", UArray_asCString(ba));
        newBlock = IoState_on_doCString_withLabel_(IOSTATE,
IoState_lobby(IOSTATE), UArray_asCString(ba), "Block readFromStore");

        if (!newBlock || !ISBLOCK(newBlock))
        {
                IoState_error_(IOSTATE, NULL, "Store found bad block code: %s",
(char *)UArray_bytes(ba));
        }

        IoBlock_copy_(self, newBlock);

        {
                PID_TYPE pid = BStream_readTaggedInt32(stream);

                if (pid)
                {
                        DATA(self)->scope = IoState_objectWithPid_(IOSTATE,
pid);
                }
                else
                {
                        DATA(self)->scope = NULL;
                }
        }
}
*/

IoBlock *IoBlock_proto(void *vState) {
    IoState *state = (IoState *)vState;
    IoMethodTable methodTable[] = {
        {"print", IoBlock_print},
        {"code", IoBlock_code},
        {"message", IoBlock_message},
        {"setMessage", IoBlock_setMessage},
        {"argumentNames", IoBlock_argumentNames},
        {"setArgumentNames", IoBlock_argumentNames_},
        {"setScope", IoBlock_setScope_},
        {"scope", IoBlock_scope},
        {"performOn", IoBlock_performOn},
        {"call", IoBlock_call},
        {"setPassStops", IoBlock_setPassStops_},
        {"passStops", IoBlock_passStops},
        {"setProfilerOn", IoBlock_setProfilerOn},
        {"profilerTime", IoBlock_profilerTime},
        {NULL, NULL},
    };

    IoObject *self = IoObject_new(state);

    IoObject_setDataPointer_(self, io_calloc(1, sizeof(IoBlockData)));
    IoObject_tag_(self, IoBlock_newTag(state));
    DATA(self)->message = IOSTATE->nilMessage;
    DATA(self)->argNames = List_new();
    DATA(self)->scope = NULL;
    IoState_registerProtoWithId_((IoState *)state, self, protoId);

    IoObject_addMethodTable_(self, methodTable);
    return self;
}

IoBlock *IoBlock_rawClone(IoBlock *proto) {
    IoBlockData *protoData = DATA(proto);
    IoObject *self = IoObject_rawClonePrimitive(proto);
    IoObject_setDataPointer_(self, io_calloc(1, sizeof(IoBlockData)));
    IoObject_tag_(self, IoObject_tag(proto));
    DATA(self)->message = IoMessage_deepCopyOf_(protoData->message);
    DATA(self)->argNames = List_clone(protoData->argNames);
    DATA(self)->scope = protoData->scope;
    IoObject_isActivatable_(self, IoObject_isActivatable(proto));
    DATA(self)->passStops = DATA(proto)->passStops;
    return self;
}

IoBlock *IoBlock_new(IoState *state) {
    IoObject *proto = IoState_protoWithId_((IoState *)state, protoId);
    return IOCLONE(proto);
}

void IoBlock_rawPrint(IoBlock *self) {
    UArray *ba = IoBlock_justCode(self);
    printf("%s\n", (char *)UArray_bytes(ba));
}

void IoBlock_mark(IoBlock *self) {
    IoBlockData *bd = DATA(self);
    IoObject_shouldMark(bd->message);
    IoObject_shouldMarkIfNonNull(bd->scope);
    LIST_DO_(bd->argNames, IoObject_shouldMark);
}

void IoBlock_free(IoBlock *self) {
    List_free(DATA(self)->argNames);
    io_free(IoObject_dataPointer(self));
}

void IoBlock_message_(IoBlock *self, IoMessage *m) {
    DATA(self)->message = IOREF(m);
}

// calling --------------------------------------------------------

IoObject *IoBlock_activateWithProfiler(IoBlock *self, IoObject *target,
                                       IoObject *locals, IoMessage *m,
                                       IoObject *slotContext) {
    clock_t profilerMark = clock();
    IoObject *result = IoBlock_activate(self, target, locals, m, slotContext);
    DATA(self)->profilerTime += clock() - profilerMark;
    return result;
}

IO_METHOD(IoBlock, setProfilerOn) {
    /*doc Block setProfilerOn(aBool)
    If aBool is true, the global block profiler is enabled, if false it is
    disabled. Returns self.
    */

    IoObject *aBool = IoMessage_locals_valueArgAt_(m, locals, 0);
    IoTag *tag = IoObject_tag(self);

    if (ISTRUE(aBool)) {
        IoTag_activateFunc_(tag,
                            (IoTagActivateFunc *)IoBlock_activateWithProfiler);
    } else {
        IoTag_activateFunc_(tag, (IoTagActivateFunc *)IoBlock_activate);
    }

    return self;
}

IoObject *IoBlock_activate(IoBlock *self, IoObject *target, IoObject *locals,
                           IoMessage *m, IoObject *slotContext) {
    IoState *state = IOSTATE;
    IoBlockData *selfData = DATA(self);
    List *argNames = selfData->argNames;
    IoObject *scope = selfData->scope;
    IoObject *blockLocals = IOCLONE(state->localsProto);
    IoObject *result;
    IoObject *callObject;

    IoObject_isLocals_(blockLocals, 1);

    if (!scope) {
        scope = target;
    }

    IoObject_createSlotsIfNeeded(blockLocals);

    callObject = IoCall_with(state, locals, target, m, slotContext, self,
                             state->currentCoroutine);

    {
        PHash *bslots = IoObject_slots(blockLocals);
        PHash_at_put_(bslots, state->callSymbol, callObject);
        PHash_at_put_(bslots, state->selfSymbol, scope);
        PHash_at_put_(bslots, state->updateSlotSymbol,
                      state->localsUpdateSlotCFunc);
    }

    IoObject_isReferenced_(blockLocals, 0);
    IoObject_isReferenced_(callObject, 0);

    LIST_FOREACH(argNames, i, name,
                 IoObject *arg =
                     IoMessage_locals_valueArgAt_(m, locals, (int)i);
                 // gc may kick in while evaling locals, so we need to be safe
                 IoObject_setSlot_to_(blockLocals, name, arg););

    if (Coro_stackSpaceAlmostGone(IoCoroutine_cid(state->currentCoroutine))) {
        /*
        IoCoroutine *currentCoroutine = state->currentCoroutine;
        Coro *coro = IoCoroutine_cid(currentCoroutine);

        printf("%p-%p block overflow %i/%i\n",
                  (void *)currentCoroutine, (void *)coro,
        Coro_bytesLeftOnStack(coro), Coro_stackSize(coro)); printf("message =
        %s\n", CSTRING(IoMessage_name(selfData->message)));
        */
        {
            IoCoroutine *newCoro = IoCoroutine_new(state);
            IoCoroutine_try(newCoro, blockLocals, blockLocals,
                            selfData->message);
            result = IoCoroutine_rawResult(newCoro);
        }
    } else {
        result = IoMessage_locals_performOn_(selfData->message, blockLocals,
                                             blockLocals);
    }

    if (DATA(self)->passStops == 0) {
        state->returnValue = result;
        state->stopStatus = IoCall_rawStopStatus(callObject);
    }

    IoState_stackRetain_(state, result);

#ifdef IO_BLOCK_LOCALS_RECYCLING
#ifndef STACK_POP_CALLBACK
    if (!IoObject_isReferenced(blockLocals) &&
        !IoObject_isReferenced(callObject)) {
        CollectorMarker_remove((CollectorMarker *)blockLocals);
        IoObject_free(blockLocals);

        CollectorMarker_remove((CollectorMarker *)callObject);
        IoObject_free(callObject);
        // IoObject_freeIfUnreferenced(blockLocals);
        // IoObject_freeIfUnreferenced(callObject);
    } else
#endif
    {
        // since the callObject doesn't IOREF its blockLocals pointer
        if (IoObject_isReferenced(callObject)) {
            IoObject_isReferenced_(blockLocals, 1);
        }
    }
#endif

    return result;
}

// ------------------------------------------------------------------------

IoObject *IoBlock_method(IoObject *target, IoObject *locals, IoMessage *m) {
    /*doc Object method(args..., body)
          Creates a method.
          <tt>args</tt> is a list of formal arguments (can be empty). <br/>
          <tt>body</tt> is evaluated in the context of Locals object.<br/>
          Locals' proto is a message receiver (i.e. self).
          <br/>
          Slot with a method is <em>activatable</em>. Use getSlot(name) to
       retrieve method object without activating it (i.e. calling). <br/> See
       also <tt>Object block</tt>.
          */

    IoBlock *const self = IoBlock_new(IoObject_state(target));
    const int nargs = IoMessage_argCount(m);
    IoMessage *const message =
        (nargs > 0) ? IoMessage_rawArgAt_(m, nargs - 1) : IOSTATE->nilMessage;
    int i;

    DATA(self)->message = IOREF(message);
    IoObject_isActivatable_(self, 1);

    for (i = 0; i < nargs - 1; i++) {
        IoMessage *argMessage = IoMessage_rawArgAt_(m, i);
        IoSymbol *name = IoMessage_name(argMessage);
        List_append_(DATA(self)->argNames, IOREF(name));
    }

    return self;
}

IoObject *IoObject_block(IoObject *target, IoObject *locals, IoMessage *m) {
    /*doc Object block(args..., body)
          Creates a block and binds it to the sender context (i.e. lexical
    context). In other words, block locals' proto is sender's locals.
          <tt>args</tt> is a list of formal arguments (can be empty). <br/>
          <tt>body</tt> is evaluated in the context of Locals object.<br/>
          See also <tt>Object method</tt>.
          <br/>
          <pre>
          Io> block(x, x*2) scope == thisContext
    ==> true
          </pre>
          */

    IoBlock *self = (IoBlock *)IoBlock_method(target, locals, m);
    DATA(self)->scope = IOREF(locals);
    IoObject_isActivatable_(self, 0);
    return self;
}

IO_METHOD(IoBlock, print) {
    /*doc Block print
    Prints an Io source code representation of the block/method.
    */

    UArray *ba = IoBlock_justCode(self);
    IoState_print_(IOSTATE, UArray_asCString(ba));
    return IONIL(self);
}

UArray *IoBlock_justCode(IoBlock *self) {
    UArray *ba = UArray_new();

    if (DATA(self)->scope) {
        UArray_appendCString_(ba, "block(");
    } else {
        UArray_appendCString_(ba, "method(");
    }

    LIST_FOREACH(DATA(self)->argNames, i, argName,
                 UArray_append_(ba, IoSeq_rawUArray((IoSymbol *)argName));
                 UArray_appendCString_(ba, ", "););

    {
        UArray *d = IoMessage_description(DATA(self)->message);
        UArray_append_(ba, d);
        UArray_free(d);
    }

    UArray_appendCString_(ba, ")");
    return ba;
}

IO_METHOD(IoBlock, code) {
    /*doc Block code
    Returns a string containing the decompiled code of the receiver.
    */

    return IoState_symbolWithUArray_copy_(IOSTATE, IoBlock_justCode(self), 0);
}

IO_METHOD(IoBlock, code_) {
    /*doc Block setCode(aString)
    Sets the receiver's message to a compiled version of aString. Returns self.
    */

    IoSymbol *string = IoMessage_locals_symbolArgAt_(m, locals, 0);
    char *s = CSTRING(string);
    IoMessage *newM =
        IoMessage_newFromText_label_(IOSTATE, s, "[IoBlock_code_]");

    if (newM) {
        DATA(self)->message = IOREF(newM);
    } else {
        IoState_error_(IOSTATE, m, "no messages found in compile string");
    }

    return self;
}

IO_METHOD(IoBlock, message) {
    /*doc Block message
    Returns the root message of the receiver.
    */

    return DATA(self)->message ? (IoObject *)DATA(self)->message : IONIL(self);
}

IO_METHOD(IoBlock, setMessage) {
    /*doc Block setMessage(aMessage)
    Sets the root message of the receiver to aMessage.
    */

    IoMessage *message = IoMessage_locals_messageArgAt_(m, locals, 0);
    DATA(self)->message = IOREF(message);
    return self;
}

IO_METHOD(IoBlock, argumentNames) {
    /*doc Block argumentNames
    Returns a List of strings containing the argument names of the receiver.
    */

    IoList *argsList = IoList_new(IOSTATE);

    LIST_FOREACH(DATA(self)->argNames, i, v,
                 IoList_rawAppend_(argsList, (IoObject *)v));

    return argsList;
}

IO_METHOD(IoBlock, argumentNames_) {
    /*doc Block setArgumentNames(aListOfStrings)
    Sets the receiver's argument names to those specified in
    aListOfStrings. Returns self.
    */

    IoList *newArgNames = IoMessage_locals_listArgAt_(m, locals, 0);
    List *rawNewArgNames = IoList_rawList(newArgNames);

    LIST_FOREACH(
        rawNewArgNames, i, v,
        IOASSERT(ISSYMBOL(((IoObject *)v)), "argument names must be Strings"););

    List_copy_(DATA(self)->argNames, IoList_rawList(newArgNames));
    return self;
}

IO_METHOD(IoBlock, scope) {
    /*doc Block scope
    Returns the scope used when the block is activated or
    Nil if the target of the message is the scope.
    */

    IoObject *scope = DATA(self)->scope;
    return scope ? scope : IONIL(self);
}

IO_METHOD(IoBlock, setScope_) {
    /*doc Block setScope(anObjectOrNil)
    If argument is an object, when the block is activated,
    it will set the proto and self slots of its locals to the specified
    object. If Nil, it will set them to the target of the message.
    */

    IoObject *scope = IoMessage_locals_valueArgAt_(m, locals, 0);
    DATA(self)->scope = ISNIL(scope) ? NULL : IOREF(scope);
    return self;
}

IO_METHOD(IoBlock, passStops) {
    /*doc Block passStops
    Returns whether or not the receiver passes return/continue/break to caller.
    */

    return IOBOOL(self, DATA(self)->passStops);
}

IO_METHOD(IoBlock, setPassStops_) {
    /*doc Block setPassStops(aBool)
    Sets whether the receiver passes return/continue/break to caller.
    */

    DATA(self)->passStops = ISTRUE(IoMessage_locals_valueArgAt_(m, locals, 0));
    return self;
}

IO_METHOD(IoBlock, performOn) {
    /*doc Block performOn(anObject, optionalLocals, optionalMessage,
    optionalSlotContext) Activates the receiver in the target context of
    anObject. Returns the result.
    */

    IoObject *bTarget = IoMessage_locals_valueArgAt_(m, locals, 0);
    IoObject *bLocals = locals;
    IoObject *bMessage = m;
    IoObject *bContext = bTarget;
    int argCount = IoMessage_argCount(m);

    if (argCount > 1) {
        bLocals = IoMessage_locals_valueArgAt_(m, locals, 1);
    }

    if (argCount > 2) {
        bMessage = IoMessage_locals_valueArgAt_(m, locals, 2);
    }

    if (argCount > 3) {
        bContext = IoMessage_locals_valueArgAt_(m, locals, 3);
    }

    return IoBlock_activate(self, bTarget, bLocals, bMessage, bContext);
}

IO_METHOD(IoBlock, call) {
    /*doc Block call(arg0, arg1, ...)
    Activates the receiver with the provided arguments.
    */

    return IoBlock_activate(self, locals, locals, m, locals);
}

IO_METHOD(IoBlock, profilerTime) {
    /*doc Block profilerTime
    Returns clock() time spent in compiler in seconds.
    */

    return IONUMBER(((double)DATA(self)->profilerTime) /
                    ((double)CLOCKS_PER_SEC));
}

void IoBlock_rawResetProfilerTime(IoBlock *self) {
    DATA(self)->profilerTime = 0;
}

clock_t IoBlock_rawProfilerTime(IoBlock *self) {
    return DATA(self)->profilerTime;
}
