
// metadoc Map category Core
// metadoc Map copyright Steve Dekorte 2002
// metadoc Map license BSD revised
/*metadoc Map description
A key/value dictionary appropriate for holding large key/value collections.
*/

/*cmetadoc Map description
Thin IoObject wrapper around basekit's PHash (cuckoo hash). DATA(self)
is the PHash*; mark walks every key and value so contained IoObjects
remain live. Unlike Io's own Object slots (which are stored in a
PHash owned by the Object), Map is a separate user-visible collection
with ordering-free semantics and symbol keys. The foreach implementation
has two paths: on the iterative evaluator it materializes keys via
IoMap_rawKeys and hands the List off to the foreach frame state machine
(controlFlow.foreachInfo.mapSource carries the original Map so the loop
can look the value up per iteration). The recursive fallback PHASH_FOREACH
is used only during VM bootstrap before state->currentFrame exists.
*/

#include "IoMap.h"
#include "IoObject.h"
#include "IoState.h"
#include "IoCFunction.h"
#include "IoSeq.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoEvalFrame.h"
#include "IoList.h"
#include "IoBlock.h"

static const char *protoId = "Map";

#define DATA(self) ((PHash *)IoObject_dataPointer(self))

int IoMap_compare(IoMap *self, IoMap *other);

/*cdoc Map IoMap_newTag(state)
Builds the Map tag and installs clone/free/mark/compare function
pointers. Stream write/read slots are left unset (the commented-out
block below preserves the legacy BStream format).
*/
IoTag *IoMap_newTag(void *state) {
    IoTag *tag = IoTag_newWithName_(protoId);
    IoTag_state_(tag, state);
    IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoMap_rawClone);
    IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoMap_free);
    IoTag_markFunc_(tag, (IoTagMarkFunc *)IoMap_mark);
    IoTag_compareFunc_(tag, (IoTagCompareFunc *)IoMap_compare);
    // IoTag_writeToStreamFunc_(tag, (IoTagWriteToStreamFunc
    // *)IoMap_writeToStream_); IoTag_readFromStreamFunc_(tag,
    // (IoTagReadFromStreamFunc *)IoMap_readFromStream_);
    return tag;
}

/*cdoc Map IoMap_compare(self, other)
Registered as the tag's compareFunc. Falls back to default pointer
comparison against non-Maps. Otherwise compares by size first, then
by walking the receiver's entries and comparing values for identical
keys; any missing key or unequal value yields a nonzero result.
*/
int IoMap_compare(IoMap *self, IoMap *other) {
    if (!ISMAP(other)) {
        return IoObject_defaultCompare(self, other);
    }

    size_t s1 = PHash_size(DATA(self));
    size_t s2 = PHash_size(DATA(other));

    if (s1 != s2) {
        return s1 > s2 ? 1 : -1;
    }

    int mismatch = 0;
    PHASH_FOREACH(DATA(self), k, v, {
        if (mismatch) continue;
        void *ov = PHash_at_(DATA(other), k);
        if (ov == NULL) {
            mismatch = 1;
        } else {
            int c = IoObject_compare((IoObject *)v, (IoObject *)ov);
            if (c) mismatch = c < 0 ? -1 : 1;
        }
    });
    return mismatch;
}

/*
void IoMap_writeToStream_(IoMap *self, BStream *stream)
{
        PHASH_FOREACH(DATA(self), k, v,
                BStream_writeTaggedInt32_(stream, IoObject_pid(k));
                BStream_writeTaggedInt32_(stream, IoObject_pid(v));
        );

        BStream_writeTaggedInt32_(stream, 0);
}

void IoMap_readFromStream_(IoMap *self, BStream *stream)
{
        PHash *hash = DATA(self);

        for (;;)
        {
                int k, v;

                k = BStream_readTaggedInt32(stream);

                if (k == 0)
                {
                        break;
                }

                v = BStream_readTaggedInt32(stream);
                PHash_at_put_(hash, IoState_objectWithPid_(IOSTATE, k),
IoState_objectWithPid_(IOSTATE, v));
        }
}
*/

/*cdoc Map IoMap_proto(state)
Creates the Map proto, attaches a fresh PHash as its data pointer,
and wires up the Io-visible method table (empty, at, atPut, keys,
values, foreach, hasKey, hasValue, removeAt, ...). Called once during
VM init; all later Maps are clones of this proto.
*/
IoMap *IoMap_proto(void *state) {
    IoMethodTable methodTable[] = {
        {"empty", IoMap_empty},       {"at", IoMap_at},
        {"atPut", IoMap_atPut},       {"atIfAbsentPut", IoMap_atIfAbsentPut},
        {"size", IoMap_size},         {"keys", IoMap_keys},
        {"values", IoMap_values},     {"foreach", IoMap_foreach},
        {"hasKey", IoMap_hasKey},     {"hasValue", IoMap_hasValue},
        {"removeAt", IoMap_removeAt}, {NULL, NULL},
    };

    IoObject *self = IoObject_new(state);

    IoObject_tag_(self, IoMap_newTag(state));
    IoObject_setDataPointer_(self, PHash_new());

    IoState_registerProtoWithId_((IoState *)state, self, protoId);

    IoObject_addMethodTable_(self, methodTable);
    return self;
}

/*cdoc Map IoMap_rawClone(proto)
Registered as the tag's cloneFunc. Gives the clone its own PHash
copy so mutation of one Map does not leak into the proto.
*/
IoMap *IoMap_rawClone(IoMap *proto) {
    IoObject *self = IoObject_rawClonePrimitive(proto);
    IoObject_setDataPointer_(self, PHash_clone(DATA(proto)));
    return self;
}

/*cdoc Map IoMap_new(state)
Convenience constructor: looks up the registered proto and clones it.
Used by C callers (serialization, foreach plumbing) that want a fresh
Map without going through message machinery.
*/
IoMap *IoMap_new(void *state) {
    IoObject *proto = IoState_protoWithId_((IoState *)state, protoId);
    return IOCLONE(proto);
}

/*cdoc Map IoMap_free(self)
Registered as the tag's freeFunc. Frees the backing PHash; contained
IoObjects are GC-managed and not touched here.
*/
void IoMap_free(IoMap *self) { PHash_free(DATA(self)); }

/*cdoc Map IoMap_mark(self)
Registered as the tag's markFunc. Walks every key and value so
contained IoObjects stay live for the GC.
*/
void IoMap_mark(IoMap *self) {
    // PHash_doOnKeyAndValue_(DATA(self), (ListDoCallback
    // *)IoObject_shouldMark);
    PHASH_FOREACH(DATA(self), k, v, IoObject_shouldMark(k);
                  IoObject_shouldMark(v));
}

/*cdoc Map IoMap_rawAtPut(self, k, v)
Low-level insert used from C. IOREFs both key and value so the GC
keeps them alive through this Map. Preferred over the Io-level atPut
when a value is being threaded through internal machinery.
*/
void IoMap_rawAtPut(IoMap *self, IoSymbol *k, IoObject *v) {
    PHash_at_put_(DATA(self), IOREF(k), IOREF(v));
}

PHash *IoMap_rawHash(IoMap *self) { return DATA(self); }

// -----------------------------------------------------------

IO_METHOD(IoMap, empty) {
    /*doc Map empty
    Removes all keys from the receiver. Returns self.
    */

    PHash_clean(DATA(self));
    return self;
}

/*cdoc Map IoMap_rawAt(self, k)
Low-level lookup used from C. Returns NULL on miss (not nil) so
callers can distinguish a stored nil from an absent key.
*/
IoObject *IoMap_rawAt(IoMap *self, IoSymbol *k) {
    return PHash_at_(DATA(self), k);
}

IO_METHOD(IoMap, at) {
    /*doc Map at(keyString, optionalDefaultValue)
    Returns the value for the key keyString. Returns nil if the key is absent.
    */

    IoSymbol *k = IoMessage_locals_symbolArgAt_(m, locals, 0);
    if (IOSTATE->errorRaised) return IONIL(self);
    void *result = PHash_at_(DATA(self), k);

    if (!result && IoMessage_argCount(m) > 1) {
        return IoMessage_locals_valueArgAt_(m, locals, 1);
    }

    return (result) ? result : IONIL(self);
}

IO_METHOD(IoMap, atPut) {
    /*doc Map atPut(keyString, aValue)
    Inserts/sets aValue with the key keyString. Returns self.
    */

    IoSymbol *k = IoMessage_locals_symbolArgAt_(m, locals, 0);
    if (IOSTATE->errorRaised) return IONIL(self);
    IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 1);
    if (IOSTATE->errorRaised) return IONIL(self);
    IoMap_rawAtPut(self, k, v);
    return self;
}

IO_METHOD(IoMap, atIfAbsentPut) {
    /*doc Map atIfAbsentPut(keyString, aValue)
    If a value is present at the specified key, the value is returned.
    Otherwise, inserts/sets aValue and returns aValue.
    */

    IoSymbol *k = IoMessage_locals_symbolArgAt_(m, locals, 0);
    if (IOSTATE->errorRaised) return IONIL(self);

    if (PHash_at_(DATA(self), k) == NULL) {
        IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 1);
        IoMap_rawAtPut(self, k, v);
    }

    return PHash_at_(DATA(self), k);
}

IO_METHOD(IoMap, size) {
    /*doc Map size
    Returns the number of key/value pairs in the receiver.
    */

    return IONUMBER(PHash_count(DATA(self)));
}

IO_METHOD(IoMap, hasKey) {
    /*doc Map hasKey(keyString)
    Returns true if the key is present or false otherwise.
    */

    IoSymbol *k = IoMessage_locals_symbolArgAt_(m, locals, 0);
    return IOBOOL(self, PHash_at_(DATA(self), k) != NULL);
}

IO_METHOD(IoMap, removeAt) {
    /*doc Map removeAt(keyString)
    Removes the specified keyString if present. Returns self.
    */

    IoSymbol *k = IoMessage_locals_symbolArgAt_(m, locals, 0);
    PHash_removeKey_(DATA(self), k);
    return self;
}

IO_METHOD(IoMap, hasValue) {
    /*doc Map hasValue(aValue)
    Returns true if the value is one of the Map's values or false otherwise.
    */

    // slow implementation

    IoList *values = IoMap_values(self, locals, m);
    return IoList_contains(values, locals, m);
}

/*cdoc Map IoMap_rawKeys(self)
Builds a List of the Map's keys in PHash iteration order. Used by
the Io-visible keys method and — importantly — by the iterative
foreach path, which needs a materialized index-addressable collection
to drive the foreach frame state machine.
*/
IoList *IoMap_rawKeys(IoMap *self) {
    IoList *list = IoList_new(IOSTATE);
    PHASH_FOREACH(DATA(self), k, v, IoList_rawAppend_(list, k));
    return list;
}

/*cdoc Map IoMap_keys(self, locals, m)
Io-visible thin wrapper over IoMap_rawKeys. Defined as a plain
function (not IO_METHOD) because IoMap_rawKeys is also called from C
paths that already have a raw Map pointer.
*/
IoList *IoMap_keys(IoMap *self, IoObject *locals, IoMessage *m) {
    /*doc Map keys
    Returns a List of the receivers keys.
    */

    return IoMap_rawKeys(self);
}

IO_METHOD(IoMap, values) {
    /*doc Map values
    Returns a List of the receivers values.
    */

    IoList *list = IoList_new(IOSTATE);
    PHASH_FOREACH(DATA(self), k, v, IoList_rawAppend_(list, v));
    return list;
}

IO_METHOD(IoMap, foreach) {
    /*doc Map foreach(optionalKey, value, message)
    For each key value pair, sets the locals key to
the key and value to the value and executes message.
Example:
<pre>	aMap foreach(k, v, writeln(k, " = ", v))
aMap foreach(v, write(v))</pre>

Example use with a block:

<pre>	myBlock = block(k, v, write(k, " = ", v, "\n"))
aMap foreach(k, v, myBlock(k, v))</pre>
*/

    IoState *state = IOSTATE;
    IoSymbol *keyName, *valueName;
    IoMessage *doMessage;

    IoMessage_foreachArgs(m, self, &keyName, &valueName, &doMessage);
    if (state->errorRaised) return IONIL(self);

    // Iterative path: build keys list for index-based iteration
    if (state->currentFrame != NULL) {
        IoEvalFrame *frame = state->currentFrame;
        IoEvalFrameData *fd = FRAME_DATA(frame);
        IoList *keysList = IoMap_rawKeys(self);
        int keyCount = (int)List_size(IoList_rawList(keysList));

        // Use keys list as collection, store original map for value lookup
        fd->controlFlow.foreachInfo.collection = (IoObject *)keysList;
        fd->controlFlow.foreachInfo.mapSource = self;
        fd->controlFlow.foreachInfo.bodyMsg = doMessage;
        fd->controlFlow.foreachInfo.indexName = keyName;
        fd->controlFlow.foreachInfo.valueName = valueName;
        fd->controlFlow.foreachInfo.currentIndex = 0;
        fd->controlFlow.foreachInfo.collectionSize = keyCount;
        fd->controlFlow.foreachInfo.lastResult = NULL;
        fd->controlFlow.foreachInfo.direction = 1;
        fd->controlFlow.foreachInfo.isEach = 0;

        fd->state = FRAME_STATE_FOREACH_EVAL_BODY;
        state->needsControlFlowHandling = 1;
        return state->ioNil;
    }

    // Recursive fallback
    PHash *p = DATA(self);
    IoObject *result = IONIL(self);
    IoState_pushRetainPool(state);

    PHASH_FOREACH(
        p, key, value, IoState_clearTopPool(state);
        if (keyName) { IoObject_setSlot_to_(locals, keyName, key); }

        IoObject_setSlot_to_(locals, valueName, value);
        IoMessage_locals_performOn_(doMessage, locals, locals);

        if (IoState_handleStatus(IOSTATE)) { break; });

    IoState_popRetainPoolExceptFor_(state, result);
    return result;
}
