
// metadoc Message category Core
// metadoc Message copyright Steve Dekorte 2002
// metadoc Message license BSD revised
/*metadoc Message description
A Message object encapsulates the action of a message send.
Blocks are composed of a Message and its children.
<p>
Terminology
<pre>
Example:
  Io> msg := message(A B(C D); E F)
  ==> A B(C D); E F

In the above example:
  msg name            =>  A
  msg next            =>  B(C D); E F
  msg next arguments  =>  list(C D)
  msg next next name  =>  ;
  msg next next next  =>  E F
</pre>
Important: Modifying the message tree of a block currently in use may cause
a crash if a garbage collection cycle occurs. If the implementation were
changed to retain every called message, this could be avoided.
But the cost to performance seems to outweigh the need to cover this case for
now.
*/

#include "IoObject.h"
#define IOMESSAGE_C
#include "IoMessage.h"
#undef IOMESSAGE_C
#include "IoSeq.h"
#include "IoMap.h"
#include "IoNumber.h"
#include "IoState.h"
#include "IoState_debug.h"
#include "IoCFunction.h"
#include "IoBlock.h"
#include "IoList.h"
#include "IoDate.h"
#include "IoSeq.h"
#include <ctype.h>
#include <stdarg.h>
#include "IoMessage_parser.h"
#include "IoMessage_opShuffle.h"

static const char *protoId = "Message";
#define DATA(self) ((IoMessageData *)IoObject_dataPointer(self))

/*
void IoMessage_writeToStream_(IoMessage *self, BStream *stream)
{
        UArray *ba = IoMessage_description(self);
        BStream_writeTaggedUArray_(stream, ba);
        UArray_free(ba);
}

void IoMessage_readFromStream_(IoMessage *self, BStream *stream)
{
        const char *code = BStream_readTaggedCString(stream);
        IoMessage *m = IoMessage_newFromText_label_(IOSTATE, (char *)code,
"[from store]"); IoMessage_copy_(self, m);
}
*/

IoObject *IoMessage_activate(IoMessage *self, IoObject *target,
                             IoObject *locals, IoMessage *m,
                             IoObject *slotContext) {
    // printf("activating self %s\n", CSTRING(IoMessage_name(self)));
    // printf("activating m %s\n", CSTRING(IoMessage_name(m)));

    return IoMessage_locals_performOn_(self, locals, locals);
    // return IoObject_perform(locals, locals, self);
}

IoTag *IoMessage_newTag(void *state) {
    IoTag *tag = IoTag_newWithName_(protoId);
    IoTag_state_(tag, state);
    IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoMessage_rawClone);
    IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoMessage_free);
    IoTag_markFunc_(tag, (IoTagMarkFunc *)IoMessage_mark);
    // IoTag_writeToStreamFunc_(tag, (IoTagWriteToStreamFunc
    // *)IoMessage_writeToStream_); IoTag_readFromStreamFunc_(tag,
    // (IoTagReadFromStreamFunc *)IoMessage_readFromStream_);
    IoTag_activateFunc_(tag, (IoTagActivateFunc *)IoMessage_activate);
    return tag;
}

IoMessage *IoMessage_proto(void *state) {
    IoMethodTable methodTable[] = {
        {"clone", IoMessage_clone},

        {"name", IoMessage_protoName},
        {"setName", IoMessage_protoSetName},

        {"next", IoMessage_next},
        {"setNext", IoMessage_setNext},
        {"isEndOfLine", IoMessage_isEOL},
        {"nextIgnoreEndOfLines", IoMessage_nextIgnoreEOLs},
        {"last", IoMessage_last},
        {"lastBeforeEndOfLine", IoMessage_lastBeforeEOL},

        {"argAt", IoMessage_argAt},
        {"arguments", IoMessage_arguments},
        {"setArguments", IoMessage_setArguments},
        {"appendArg", IoMessage_appendArg},
        {"appendCachedArg", IoMessage_appendCachedArg},
        {"argCount", IoMessage_argCount_},

        {"cachedResult", IoMessage_cachedResult},
        {"setCachedResult", IoMessage_setCachedResult},
        {"removeCachedResult", IoMessage_removeCachedResult},
        {"hasCachedResult", IoMessage_hasCachedResult},

        {"lineNumber", IoMessage_lineNumber},
        {"setLineNumber", IoMessage_setLineNumber},

        {"characterNumber", IoMessage_characterNumber},
        {"setCharacterNumber", IoMessage_setCharacterNumber},

        {"label", IoMessage_label},
        {"setLabel", IoMessage_setLabel},

        {"code", IoMessage_descriptionString},
        {"doInContext", IoMessage_doInContext},
        {"fromString", IoMessage_fromString},
        {"argsEvaluatedIn", IoMessage_argsEvaluatedIn},
        {"asString", IoMessage_asString},

        {"asMessageWithEvaluatedArgs", IoMessage_asMessageWithEvaluatedArgs},

        {"opShuffle", IoMessage_opShuffle},
        {"opShuffleC", IoMessage_opShuffle},

#ifdef IOMESSAGE_HASPREV
        {"previous", IoMessage_previous},
#endif

        {NULL, NULL},
    };

    IoObject *self = IoObject_new(state);
    IoMessageData *d;
    IoObject_setDataPointer_(self, io_calloc(1, sizeof(IoMessageData)));
    d = IoObject_dataPointer(self);

    IoObject_tag_(self, IoMessage_newTag(state));
    d->args = List_new();
    d->name = IOSYMBOL("[unnamed]");
    d->label = IOSYMBOL("[unlabeled]");
    // d->charNumber = -1;
    d->lineNumber = -1;
    IoState_registerProtoWithId_((IoState *)state, self, protoId);

    IoObject_addMethodTable_(self, methodTable);
    return self;
}

IoMessage *IoMessage_rawClone(IoMessage *proto) {
    IoObject *self = IoObject_rawClonePrimitive(proto);

    IoObject_setDataPointer_(self, io_calloc(1, sizeof(IoMessageData)));
    DATA(self)->args = List_new();
    IoMessage_rawSetName_(self, DATA(proto)->name);
    IoMessage_rawSetLabel_(self, DATA(proto)->label);
    // DATA(self)->name = DATA(proto)->name;
    // DATA(self)->label = DATA(proto)->label;
    /* any clone really needs to be a deep copy */
    return self;
}

IoMessage *IoMessage_new(void *state) {
    IoObject *proto = IoState_protoWithId_((IoState *)state, protoId);
    IoObject *self = IOCLONE(proto);
    return self;
}

// Message shallowCopy := method(Message clone setName(name)
// setArguments(arguments))

void IoMessage_copy_(IoMessage *self, IoMessage *other) {
    IoMessage_rawSetName_(self, DATA(other)->name);

    {
        List *l1 = DATA(self)->args;
        List *l2 = DATA(other)->args;
        size_t i, max = List_size(l2);
        List_removeAll(l1);

        for (i = 0; i < max; i++) {
            List_append_(l1, IOREF(List_rawAt_(l2, i)));
        }
    }

    IoMessage_rawSetNext_(self, DATA(other)->next);
    IoMessage_rawSetCachedResult_(self, DATA(other)->cachedResult);
    IoMessage_rawCopySourceLocation(self, other);
}

void IoMessage_rawCopySourceLocation(IoMessage *self, IoMessage *other) {
    // DATA(self)->charNumber = DATA(other)->charNumber;
    DATA(self)->lineNumber = DATA(other)->lineNumber;
    IoMessage_rawSetLabel_(self, DATA(other)->label);
}

IoMessage *IoMessage_deepCopyOf_(IoMessage *self) {
    IoMessage *child = IoMessage_new(IOSTATE);
    int i;

    /*printf("deep copying: %s\n",
     * UArray_asCString(IoMessage_description(self)));*/
    for (i = 0; i < IoMessage_argCount(self); i++) {
        List_append_(DATA(child)->args, IOREF(IoMessage_deepCopyOf_(
                                            LIST_AT_(DATA(self)->args, i))));
    }

    IoMessage_rawSetName_(child, DATA(self)->name);
    IoMessage_rawSetCachedResult_(child, (IoObject *)DATA(self)->cachedResult);

    if (DATA(self)->next) {
        IoMessage_rawSetNext_(child, IoMessage_deepCopyOf_(DATA(self)->next));
    }
    /*printf("deep copy result: %s\n",
     * UArray_asCString(IoMessage_description(child)));*/
    return child;
}

IoMessage *IoMessage_newWithName_(void *state, IoSymbol *symbol) {
    IoMessage *self = IoMessage_new(state);
    IoMessage_rawSetName_(self, symbol);
    return self;
}

IoMessage *IoMessage_newWithName_label_(void *state, IoSymbol *symbol,
                                        IoSymbol *label) {
    IoMessage *self = IoMessage_new(state);
    IoMessage_rawSetName_(self, symbol);
    IoMessage_rawSetLabel_(self, label);
    return self;
}

IoMessage *IoMessage_newWithName_returnsValue_(void *state, IoSymbol *symbol,
                                               IoObject *v) {
    IoMessage *self = IoMessage_new(state);
    IoMessage_rawSetName_(self, symbol);
    IoMessage_rawSetCachedResult_(self, v);
    return self;
}

IoMessage *IoMessage_newWithName_andCachedArg_(void *state, IoSymbol *symbol,
                                               IoObject *arg) {
    IoMessage *self = IoMessage_newWithName_(state, symbol);
    IoMessage_addCachedArg_(self, arg);
    return self;
}

void IoMessage_mark(IoMessage *self) {
    IoObject_shouldMarkIfNonNull(DATA(self)->name);
    IoObject_shouldMarkIfNonNull(DATA(self)->cachedResult);

    if (DATA(self)->args) {
        LIST_FOREACH(DATA(self)->args, i, v, IoObject_shouldMark(v));
    }

    IoObject_shouldMarkIfNonNull((IoObject *)DATA(self)->next);
    IoObject_shouldMarkIfNonNull((IoObject *)DATA(self)->label);
}

void IoMessage_free(IoMessage *self) {
    // IoMessageData *d = (IoMessageData *)IoObject_dataPointer(self);

    if (DATA(self)->args) {
        List_free(DATA(self)->args);
    }

    io_free(IoObject_dataPointer(self));
}

List *IoMessage_args(IoMessage *self) { return DATA(self)->args; }

void IoMessage_rawSetCachedResult_(IoMessage *self, IoObject *v) {
    DATA(self)->cachedResult = v ? IOREF(v) : NULL;
}

void IoMessage_rawSetName_(IoMessage *self, IoObject *v) {
    DATA(self)->name = v ? IOREF(v) : NULL;
}

void IoMessage_rawSetLabel_(IoMessage *self, IoObject *v) {
    DATA(self)->label = v ? IOREF(v) : NULL;
}

void IoMessage_label_(IoMessage *self,
                      IoSymbol *ioSymbol) /* sets label for children too */
{
    IoMessage_rawSetLabel_(self, ioSymbol);
    List_do_with_(DATA(self)->args, (ListDoWithCallback *)IoMessage_label_,
                  ioSymbol);

    if (DATA(self)->next) {
        IoMessage_label_(DATA(self)->next, ioSymbol);
    }
}

int IoMessage_rawLineNumber(IoMessage *self) { return DATA(self)->lineNumber; }

void IoMessage_rawSetLineNumber_(IoMessage *self, int n) {
    DATA(self)->lineNumber = n;
}

void IoMessage_rawSetCharNumber_(IoMessage *self, int n) {
    // DATA(self)->charNumber = n;
}

int IoMessage_rawCharNumber(IoMessage *self) {
    return 0; // DATA(self)->charNumber;
}

List *IoMessage_rawArgList(IoMessage *self) { return DATA(self)->args; }

unsigned char IoMessage_isNotCached(IoMessage *self) {
    return !(DATA(self)->cachedResult);
}

unsigned char IoMessage_needsEvaluation(IoMessage *self) {
    List *args = DATA(self)->args;
    int a =
        List_detect_(args, (ListDetectCallback *)IoMessage_isNotCached) != NULL;

    if (a) {
        return 1;
    }

    if (DATA(self)->next && IoMessage_needsEvaluation(DATA(self)->next)) {
        return 1;
    }

    return 0;
}

void IoMessage_addCachedArg_(IoMessage *self, IoObject *v) {
    IoMessage *m = IoMessage_new(IOSTATE);
    IoMessage_rawSetCachedResult_(m, v);
    IoMessage_addArg_(self, m);
}

void IoMessage_setCachedArg_to_(IoMessage *self, int n, IoObject *v) {
    IoMessage *arg;

    while (!(arg = List_at_(DATA(self)->args, n))) {
        IoMessage_addArg_(self, IoMessage_new(IOSTATE));
    }

    IoMessage_rawSetCachedResult_(arg, v);
}

void IoMessage_setCachedArg_toInt_(IoMessage *self, int n, int anInt) {
    // optimized to avoid creating a number unless necessary

    IoMessage *arg = NULL;

    while (!(arg = List_at_(DATA(self)->args, n))) {
        List_append_(DATA(self)->args, IOREF(IoMessage_new(IOSTATE)));
    }

    IoMessage_rawSetCachedResult_(arg, IONUMBER(anInt));
}

IO_METHOD(IoMessage, lineNumber) {
    /*doc Message lineNumber
    Returns the line number of the message. The character number
    is typically the line number in the source text from which the message was
    read.
    */

    return IONUMBER(DATA(self)->lineNumber);
}

IO_METHOD(IoMessage, setLineNumber) {
    /*doc Message setLineNumber(aNumber)
    Sets the line number of the message. Returns self.
    */

    DATA(self)->lineNumber = IoMessage_locals_intArgAt_(m, locals, 0);
    return self;
}

IO_METHOD(IoMessage, characterNumber) {
    /*doc Message characterNumber
    Returns the message character number. The character number is typically
    the beginning character index in the source text from which the message was
    read.
    */

    return IONUMBER(0);
    // return IONUMBER(DATA(self)->charNumber);
}

IO_METHOD(IoMessage, setCharacterNumber) {
    /*doc Message setCharacterNumber(aNumber)
    Sets the character number of the message. Returns self.
    */

    // DATA(self)->charNumber = IoMessage_locals_intArgAt_(m , locals, 0);
    return self;
}

IO_METHOD(IoMessage, label) {
    /*doc Message label
    Returns the message label. The label is typically set to the
    name of the file from which the source code for the message was read.
    */

    return DATA(self)->label;
}

IO_METHOD(IoMessage, setLabel) {
    /*doc Message setLabel(aString)
    Sets the label of the message and its children. Returns self.
    */

    IoMessage_label_(self, IoMessage_locals_symbolArgAt_(m, locals, 0));
    return self;
}

// --- perform --------------------------------------------------------

IO_METHOD(IoMessage, doInContext) {
    /*doc Message doInContext(anObject, locals)
    Evaluates the receiver in the context of anObject. Optional <tt>locals</tt>
    object is used as message sender. <tt>anObject</tt> is used as sender
    otherwise.
    */

    IoObject *context = IoMessage_locals_valueArgAt_(m, (IoObject *)locals, 0);
    if (IoMessage_argCount(m) >= 2) {
        locals = IoMessage_locals_valueArgAt_(m, (IoObject *)locals, 1);
    } else {
        // Default to using the context as the locals so that the common case
        // of, call argAt(2) doInContext(call sender) is easier.
        locals = context;
    }
    return IoMessage_locals_performOn_(self, locals, context);
}

//#define IO_DEBUG_STACK

IoObject *IoMessage_locals_performOn_(IoMessage *self, IoObject *locals,
                                      IoObject *target) {
    IoState *state = IOSTATE;
    IoMessage *m = self;
    IoObject *result = target;
    IoObject *cachedTarget = target;
    // IoObject *semicolonSymbol = state->semicolonSymbol;
    // IoMessageData *md;
    IoMessageData *md;

    if (state->receivedSignal) {
        IoState_callUserInterruptHandler(IOSTATE);
    }

    do {
        // md = DATA(m);

        // printf("%s %i\n", CSTRING(IoMessage_name(m)), state->stopStatus);
        // printf(" %s\n", CSTRING(IoMessage_name(m)));
        if (state->showAllMessages) {
            printf("M:%s:%s:%i\n", CSTRING(IoMessage_name(m)),
                   CSTRING(IoMessage_rawLabel(m)), IoMessage_rawLineNumber(m));
        }

        md = DATA(m);

        if (md->name == state->semicolonSymbol) {
            target = cachedTarget;
        } else {
            result = md->cachedResult; // put it on the stack?
            /*
            if(state->debugOn)
            {
                    char *s = CSTRING(DATA(m)->name);
                    printf("%s\n", s);
                    if (strcmp(s, "clone") == 0)
                    {
                            printf("found '%s'\n", s);
                    }
            }
            */

            if (!result) {
                IoState_pushRetainPool(state);
#ifdef IOMESSAGE_INLINE_PERFORM
                if (IoObject_tag(target)->performFunc == NULL) {
                    result = IoObject_perform(target, locals, m);
                } else {
                    result =
                        IoObject_tag(target)->performFunc(target, locals, m);
                }
#else
                result = IoObject_tag(target)->performFunc(target, locals, m);
#endif
                IoState_popRetainPoolExceptFor_(state, result);
            }

            // IoObject_freeIfUnreferenced(target);
            target = result;

            if (state->stopStatus != MESSAGE_STOP_STATUS_NORMAL) {
                return state->returnValue;
                /*
                result = state->returnValue;

                if (result)
                {
                        //IoState_stackRetain_(state, result);
                        return result;
                }
                printf("IoBlock no result!\n");
                return state->ioNil;
                */
            }
        }
    } while ((m = md->next));

    return result;
}

// getting arguments ---------------------------

int IoMessage_argCount(IoMessage *self) {
    return (int)List_size(DATA(self)->args);
}

void IoMessage_assertArgCount_receiver_(IoMessage *self, int n,
                                        IoObject *receiver) {
    if (List_size(DATA(self)->args) < n) {
        IoState_error_(IOSTATE, self, "[%s %s] requires %i arguments\n",
                       IoObject_name(receiver), CSTRING(DATA(self)->name), n);
    }
}

void IoMessage_locals_numberArgAt_errorForType_(IoMessage *self,
                                                IoObject *locals, int n,
                                                const char *typeName) {
    IoObject *v = IoMessage_locals_valueArgAt_(self, locals, n);
    IoState_error_(IOSTATE, self,
                   "argument %i to method '%s' must be a %s, not a '%s'", n,
                   CSTRING(DATA(self)->name), typeName, IoObject_name(v));
}

IoObject *IoMessage_locals_numberArgAt_(IoMessage *self, IoObject *locals,
                                        int n) {
    IoObject *v = IoMessage_locals_valueArgAt_(self, locals, n);

    if (!ISNUMBER(v)) {
        IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "Number");
    }

    return v;
}

int IoMessage_locals_boolArgAt_(IoMessage *self, IoObject *locals, int n) {
    IoObject *v = IoMessage_locals_valueArgAt_(self, locals, n);

    return !ISNIL(v) && !ISFALSE(v);
}

int IoMessage_locals_intArgAt_(IoMessage *self, IoObject *locals, int n) {
    return IoNumber_asInt(IoMessage_locals_numberArgAt_(self, locals, n));
}

long IoMessage_locals_longArgAt_(IoMessage *self, IoObject *locals, int n) {
    return IoNumber_asLong(IoMessage_locals_numberArgAt_(self, locals, n));
}

size_t IoMessage_locals_sizetArgAt_(IoMessage *self, IoObject *locals, int n) {
    long v = IoNumber_asLong(IoMessage_locals_numberArgAt_(self, locals, n));

    if (v < 0) {
        IoState_error_(IOSTATE, self,
                       "IoMessage_locals_sizetArgAt_ attempt to get size_t "
                       "value from negative number %i",
                       v);
        return 0;
    }

    return v;
}

double IoMessage_locals_doubleArgAt_(IoMessage *self, IoObject *locals, int n) {
    return IoNumber_asDouble(IoMessage_locals_numberArgAt_(self, locals, n));
}

float IoMessage_locals_floatArgAt_(IoMessage *self, IoObject *locals, int n) {
    return (float)IoNumber_asDouble(
        IoMessage_locals_numberArgAt_(self, locals, n));
}

char *IoMessage_locals_cStringArgAt_(IoMessage *self, IoObject *locals, int n) {
    return CSTRING(IoMessage_locals_symbolArgAt_(self, locals, n));
}

IoObject *IoMessage_locals_seqArgAt_(IoMessage *self, IoObject *locals, int n) {
    IoObject *v = IoMessage_locals_valueArgAt_(self, locals, n);

    if (!ISSEQ(v)) {
        IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "Sequence");
    }

    return v;
}

IoObject *IoMessage_locals_valueAsStringArgAt_(IoMessage *self,
                                               IoObject *locals, int n) {
    return IoObject_asString_(IoMessage_locals_valueArgAt_(self, locals, n),
                              self);
}

IoObject *IoMessage_locals_symbolArgAt_(IoMessage *self, IoObject *locals,
                                        int n) {
    IoObject *v = IoMessage_locals_valueArgAt_(self, locals, n);

    if (!ISSEQ(v)) {
        IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "Sequence");
    }

    return IoSeq_rawAsSymbol(v);
}

IoObject *IoMessage_locals_mutableSeqArgAt_(IoMessage *self, IoObject *locals,
                                            int n) {
    IoObject *v = IoMessage_locals_valueArgAt_(self, locals, n);

    if (!ISMUTABLESEQ(v)) {
        IoMessage_locals_numberArgAt_errorForType_(self, locals, n,
                                                   "mutable Sequence");
    }

    return v;
}

IoObject *IoMessage_locals_blockArgAt_(IoMessage *self, IoObject *locals,
                                       int n) {
    IoObject *v = IoMessage_locals_valueArgAt_(self, locals, n);
    if (!ISBLOCK(v))
        IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "Block");
    return v;
}

IoObject *IoMessage_locals_dateArgAt_(IoMessage *self, IoObject *locals,
                                      int n) {
    IoObject *v = IoMessage_locals_valueArgAt_(self, locals, n);
    if (!ISDATE(v))
        IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "Date");
    return v;
}

IoObject *IoMessage_locals_messageArgAt_(IoMessage *self, IoObject *locals,
                                         int n) {
    IoObject *v = IoMessage_locals_valueArgAt_(self, locals, n);
    if (!ISMESSAGE(v))
        IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "Message");
    return v;
}

IoObject *IoMessage_locals_listArgAt_(IoMessage *self, IoObject *locals,
                                      int n) {
    IoObject *v = IoMessage_locals_valueArgAt_(self, locals, n);
    if (!ISLIST(v))
        IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "List");
    return v;
}

IoObject *IoMessage_locals_mapArgAt_(IoMessage *self, IoObject *locals, int n) {
    IoObject *v = IoMessage_locals_valueArgAt_(self, locals, n);
    if (!ISMAP(v))
        IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "Map");
    return v;
}

// printing

void IoMessage_print(IoMessage *self) {
    UArray *ba = IoMessage_description(self);

    // printf("%s\n", UArray_asCString(ba));
    IoState_print_(IOSTATE, UArray_asCString(ba));
    UArray_free(ba);
}

void IoMessage_printWithReturn(IoMessage *self) {
    IoMessage_print(self);
    IoState_print_(IOSTATE, "\n");
}

UArray *IoMessage_description(IoMessage *self) {
    UArray *ba = UArray_new();
    IoMessage_appendDescriptionTo_follow_(self, ba, 1);
    return ba;
}

UArray *IoMessage_descriptionJustSelfAndArgs(IoMessage *self) {
    UArray *ba = UArray_new();
    IoMessage_appendDescriptionTo_follow_(self, ba, 0);
    return ba;
}

IO_METHOD(IoMessage, asString) {
    /*doc Message asString
    Same as code().
    */

    return IoMessage_descriptionString(self, locals, m);
}

void IoMessage_appendDescriptionTo_follow_(IoMessage *self, UArray *ba,
                                           int follow) {
    do {
        IoMessageData *data = DATA(self);

        UArray_appendCString_(ba, CSTRING(data->name));

        {
            size_t i, max = List_size(DATA(self)->args);

            if (max > 0) {
                UArray_appendCString_(ba, "(");

                for (i = 0; i < max; i++) {
                    IoMessage *arg = List_at_(DATA(self)->args, i);
                    IoMessage_appendDescriptionTo_follow_(arg, ba, 1);

                    if (i != max - 1) {
                        UArray_appendCString_(ba, ", ");
                    }
                }

                UArray_appendCString_(ba, ")");
            }
        }

        if (!follow) {
            return;
        }

        if (DATA(self)->next && DATA(self)->name != IOSTATE->semicolonSymbol)
            UArray_appendCString_(ba, " ");
        if (DATA(self)->name == IOSTATE->semicolonSymbol)
            UArray_appendCString_(ba, "\n");
    } while ((self = DATA(self)->next));
}

//  methods ---------------------------------------------------

IO_METHOD(IoMessage, clone) {
    /*doc Message clone
    Returns a Message that is a deep copy of the receiver.
    */

    return IoMessage_deepCopyOf_(self);
}

IO_METHOD(IoMessage, protoName) {
    /*doc Message name
    Returns the name of the receiver.
    */

    IoObject *s = DATA(self)->name;
    return s;
}

IO_METHOD(IoMessage, protoSetName) {
    /*doc Message setName(aString)
    Sets the name of the receiver. Returns self.
    */
    IoMessage_rawSetName_(self, IoMessage_locals_symbolArgAt_(m, locals, 0));
    // IoMessage_cacheIfPossible(self);
    return self;
}

IO_METHOD(IoMessage, descriptionString) {
    /*doc Message code
    Returns a String containing a decompiled code representation of the
    receiver.
    */

    UArray *ba =
        IoMessage_description(self); /* me must io_free the returned UArray */
    return IoState_symbolWithUArray_copy_(IOSTATE, ba, 0);
}

// next -------------------------

IO_METHOD(IoMessage, next) {
    /*doc Message next
    Returns the next message in the message chain or nil if there is no next
    message.
    */

    return DATA(self)->next ? (IoObject *)DATA(self)->next : IONIL(self);
}

IoMessage *IoMessage_rawNext(IoMessage *self) { return DATA(self)->next; }

IO_METHOD(IoMessage, setNext) {
    /*doc Message setNext(aMessageOrNil)
    Sets the next message in the message chain to a deep copy of
    aMessage or it removes the next message if aMessage is nil.
    */

    IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 0);
    IOASSERT(ISMESSAGE(v) || ISNIL(v), "argument must be Message or Nil");

    if (ISNIL(v)) {
        v = NULL;
    }

    IoMessage_rawSetNext_(self, v);
    return self;
}

void IoMessage_rawSetNext_(IoMessage *self, IoMessage *m) {
    DATA(self)->next = m ? IOREF(m) : NULL;

#ifdef IOMESSAGE_HASPREV
    if (m) {
        DATA(m)->previous = self;
    }
#endif
}

IO_METHOD(IoMessage, isEOL) {
    /*doc Message isEndOfLine
    Returns true if the message marks the end of the line. A ';' message.
    */

    return IOBOOL(self, IoMessage_rawIsEOL(self));
}

int IoMessage_rawIsEOL(IoMessage *self) {
    return IoMessage_name(self) == IOSTATE->semicolonSymbol;
}

IoMessage *IoMessage_rawNextIgnoreEOLs(IoMessage *self) {
    IoMessage *next = IoMessage_rawNext(self);

    while (next && IoMessage_rawIsEOL(next)) {
        next = IoMessage_rawNext(next);
    }

    return next;
}

IO_METHOD(IoMessage, nextIgnoreEOLs) {
    /*doc Message nextIgnoreEndOfLines
    Returns the next message in the message chain which is not an
    EndOfLine or nil if there is no next message.
    */

    IoMessage *next = IoMessage_rawNextIgnoreEOLs(self);
    return next ? next : IONIL(self);
}

IoMessage *IoMessage_rawLastBeforeEOL(IoMessage *self) {
    IoMessage *last = self;
    IoMessage *next;

    while ((next = IoMessage_rawNext(last))) {
        if (IoMessage_rawIsEOL(next)) {
            break;
        }
        last = next;
    }

    return last;
}

IO_METHOD(IoMessage, lastBeforeEOL) {
    /*doc Message lastBeforeEndOfLine
    Returns the last message in the chain before the EndOfLine or nil.
    */

    return IoMessage_rawLastBeforeEOL(self);
}

IoMessage *IoMessage_rawLast(IoMessage *self) {
    IoMessage *last = self;
    IoMessage *next;

    while ((next = IoMessage_rawNext(last))) {
        last = next;
    }

    return last;
}

IO_METHOD(IoMessage, last) {
    /*doc Message last
    Returns the last message in the chain.
    */

    return IoMessage_rawLast(self);
}

// previous -------------------------

IO_METHOD(IoMessage, previous) {
    /*doc Message previous
    Returns the previous message in the message chain or Nil if there is no
    previous message.
    */

#ifdef IOMESSAGE_HASPREV
    return DATA(self)->previous ? (IoObject *)DATA(self)->previous
                                : IONIL(self);
#else
    return IONIL(self);
#endif
}

IoMessage *IoMessage_rawPrevious(IoMessage *self) {
#ifdef IOMESSAGE_HASPREV
    return DATA(self)->previous;
#else
    return IONIL(self);
#endif
}

IO_METHOD(IoMessage, setPrevious) {
    /*doc Message setPrevious(aMessageOrNil)
    Sets the previous message in the message chain to a deep copy of
    aMessage or it removes the previous message if aMessage is Nil.
    */

    IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 0);
    IOASSERT(ISMESSAGE(v) || ISNIL(v), "argument must be Message or Nil");

    if (ISNIL(v)) {
        v = NULL;
    }

    IoMessage_rawSetPrevious(self, v);

    return self;
}

void IoMessage_rawSetPrevious(IoMessage *self, IoMessage *m) {
#ifdef IOMESSAGE_HASPREV
    DATA(self)->previous = m ? IOREF(m) : NULL;

    if (m) {
        DATA(m)->next = self;
    }
#endif
}

// ------------------------------------------------------

IO_METHOD(IoMessage, argAt) {
    /*doc Message argAt(indexNumber)
    Returns Message object for the specified argument or Nil if none exists.
    */

    int index = IoNumber_asInt(IoMessage_locals_numberArgAt_(m, locals, 0));
    IoObject *v = List_at_(DATA(self)->args, index);
    return v ? v : IONIL(self);
}

IO_METHOD(IoMessage, arguments) {
    /*doc Message arguments
    Returns a list of the message objects that act as the
    receiver's arguments. Modifying this list will not alter the actual
    list of arguments. Use the arguments_() method to do that.
    */

    IoList *argsList = IoList_new(IOSTATE);
    IoList_rawAddBaseList_(argsList, DATA(self)->args);
    return argsList;
}

IO_METHOD(IoMessage, setArguments) {
    /*doc Message setArguments(aListOfMessages)
    Sets the arguments of the receiver to deep copies of
    those contained in aListOfMessages.  Returns self.
    */

    IoList *ioList = IoMessage_locals_listArgAt_(m, locals, 0);
    List *newArgs = IoList_rawList(ioList);

    List_removeAll(DATA(self)->args);

    LIST_FOREACH(
        newArgs, i, argMessage,

        if (!ISMESSAGE((IoMessage *)argMessage)) {
            IoState_error_(
                IOSTATE, m,
                "arguments_() takes a list containing only Message objects");
        }

        List_append_(DATA(self)->args, IOREF((IoMessage *)argMessage)););

    return self;
}

IO_METHOD(IoMessage, appendArg) {
    /*doc Message appendArg(aMessage)
    Adds aMessage to the argument list of receiver. Examples:
    <pre>
    Io> message(a) appendArg(message(b))
    ==> a(b)

    Io> message(a(1,2)) appendArg(message(3))
    ==> a(1, 2, 3)
    </pre>
    */

    IoMessage *msg = IoMessage_locals_messageArgAt_(m, locals, 0);
    IoMessage_addArg_(self, msg);
    return self;
}

IO_METHOD(IoMessage, appendCachedArg) {
    /*doc Message appendCachedArg(aValue)
    Adds aValue to the argument list of receiver as a cachedResult.
    */

    IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 0);
    IoMessage_addCachedArg_(self, v);
    return self;
}

IO_METHOD(IoMessage, argCount_) {
    /*doc Message argCount
    Returns the number of arguments this message has. A faster way to do, msg
    arguments size. Examples, <pre> Io> message(a(1,2,3)) argCount
    ==> 3

    Io> message(a) argCount
    ==> 0
    </pre>
    */

    return IONUMBER(IoMessage_argCount(self));
}

IO_METHOD(IoMessage, fromString) {
    /*doc Message fromString(aString)
    Returns a new Message object for the compiled(but not executed)
    result of aString.
    */

    IoSymbol *string = IoMessage_locals_symbolArgAt_(m, locals, 0);
    IoSymbol *label = DATA(m)->label;

    if (IoMessage_argCount(m) > 1) {
        label = IoMessage_locals_symbolArgAt_(m, locals, 1);
    }

    return IoMessage_newFromText_labelSymbol_(IOSTATE, CSTRING(string), label);
}

IO_METHOD(IoMessage, cachedResult) {
    /*doc Message cachedResult
    Returns the cached result of the Message or Nil if there is none.
    */

    return (DATA(self)->cachedResult ? DATA(self)->cachedResult : IONIL(self));
}

IO_METHOD(IoMessage, setCachedResult) {
    /*doc Message setCachedResult(anObject)
    Sets the cached result of the message. Returns self.
    */
    IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 0);
    IoMessage_rawSetCachedResult_(self, v);
    return self;
}

IO_METHOD(IoMessage, removeCachedResult) {
    /*doc Message removeCachedResult
    Removes the cached result of the Message.
    */
    IoMessage_rawSetCachedResult_(self, 0x0);
    return self;
}

IO_METHOD(IoMessage, hasCachedResult) {
    /*doc Message hasCachedResult
    Returns true if there is a cached result. Nil is a valid cached result.
    */

    return IOBOOL(self, IoMessage_rawCachedResult(self) != NULL);
}

IO_METHOD(IoMessage, argsEvaluatedIn) {
    /*doc Message argsEvaluatedIn(anObject)
    Returns a List containing the argument messages evaluated in the
    context of anObject.
    */

    IoObject *context = IoMessage_locals_valueArgAt_(m, locals, 0);
    IoList *args = IoList_new(IOSTATE);
    int i;

    for (i = 0; i < List_size(DATA(self)->args); i++) {
        IoObject *arg = IoMessage_locals_valueArgAt_(self, context, i);
        IoList_rawAppend_(args, arg);
    }
    return args;
}

IO_METHOD(IoMessage, evaluatedArgs) {
    /*doc Message evaluatedArgs
    Returns a List containing the argument messages evaluated in the context.
    */

    IoList *args = IoList_new(IOSTATE);
    int i;

    for (i = 0; i < List_size(DATA(self)->args); i++) {
        IoObject *arg = IoMessage_locals_valueArgAt_(self, locals, i);
        IoList_rawAppend_(args, arg);
    }

    return args;
}

// ------------------------------

/*
IoSymbol *IoMessage_name(IoMessage *self)
{
        return DATA(self)->name;
}
*/

IoSymbol *IoMessage_rawLabel(IoMessage *self) { return DATA(self)->label; }

List *IoMessage_rawArgs(IoMessage *self) { return DATA(self)->args; }

IoMessage *IoMessage_rawArgAt_(IoMessage *self, int n) {
    IoMessage *result = List_at_(DATA(self)->args, n);
    IoState_stackRetain_(IOSTATE, result);
    return result;
}

void IoMessage_addArg_(IoMessage *self, IoMessage *m) {
    List_append_(DATA(self)->args, IOREF(m));
}

// -------------------------------

UArray *IoMessage_asMinimalStackEntryDescription(IoMessage *self) {
    IoSymbol *name = IoMessage_name(self);
    IoSymbol *label = IoMessage_rawLabel(self);
    int lineNumber = IoMessage_rawLineNumber(self);
    return UArray_newWithFormat_("%s:%i %s", CSTRING(label), lineNumber,
                                 CSTRING(name));
}

void IoMessage_foreachArgs(IoMessage *self, IoObject *receiver,
                           IoSymbol **indexSlotName, IoSymbol **valueSlotName,
                           IoMessage **doMessage) {
    int offset;

    IoMessage_assertArgCount_receiver_(self, 2, receiver);

    if (IoMessage_argCount(self) > 2) {
        *indexSlotName = IoMessage_name(IoMessage_rawArgAt_(self, 0));
        offset = 1;
    } else {
        *indexSlotName = NULL; // IONIL(self);
        offset = 0;
    }

    *valueSlotName = IoMessage_name(IoMessage_rawArgAt_(self, 0 + offset));
    *doMessage = IoMessage_rawArgAt_(self, 1 + offset);
}

IoMessage *IoMessage_asMessageWithEvaluatedArgs(IoMessage *self,
                                                IoObject *locals,
                                                IoMessage *m) {
    /*doc Message asMessageWithEvaluatedArgs(optionalContext)
          Returns a copy of receiver with arguments evaluated in the context of
       sender if optionalContext is nil.
          */
    IoState *state = IOSTATE;
    IoMessage *sendMessage;
    int i, max = IoMessage_argCount(self);
    IoObject *context = locals;

    if (IoMessage_argCount(m) > 0) {
        context = IoMessage_locals_valueArgAt_(m, locals, 0);
    }

    if (!IoMessage_needsEvaluation(self)) {
        return self;
    }

    sendMessage = IoMessage_newWithName_(state, IoMessage_name(self));
    for (i = 0; i < max; i++) {
        IoMessage *arg = IoMessage_rawArgAt_(self, i);
        IoObject *result = IoMessage_locals_performOn_(arg, context, context);
        IoMessage_setCachedArg_to_(sendMessage, i, result);
    }

    return sendMessage;
}
