
// metadoc Map category Core
// metadoc Map copyright Steve Dekorte 2002
// metadoc Map license BSD revised
/*metadoc Map description
A key/value dictionary appropriate for holding large key/value collections.
*/

#include "IoMap.h"
#include "IoObject.h"
#include "IoState.h"
#include "IoCFunction.h"
#include "IoSeq.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoList.h"
#include "IoBlock.h"

static const char *protoId = "Map";

#define DATA(self) ((PHash *)IoObject_dataPointer(self))

IoTag *IoMap_newTag(void *state) {
    IoTag *tag = IoTag_newWithName_(protoId);
    IoTag_state_(tag, state);
    IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoMap_rawClone);
    IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoMap_free);
    IoTag_markFunc_(tag, (IoTagMarkFunc *)IoMap_mark);
    // IoTag_writeToStreamFunc_(tag, (IoTagWriteToStreamFunc
    // *)IoMap_writeToStream_); IoTag_readFromStreamFunc_(tag,
    // (IoTagReadFromStreamFunc *)IoMap_readFromStream_);
    return tag;
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

IoMap *IoMap_rawClone(IoMap *proto) {
    IoObject *self = IoObject_rawClonePrimitive(proto);
    IoObject_setDataPointer_(self, PHash_clone(DATA(proto)));
    return self;
}

IoMap *IoMap_new(void *state) {
    IoObject *proto = IoState_protoWithId_((IoState *)state, protoId);
    return IOCLONE(proto);
}

void IoMap_free(IoMap *self) { PHash_free(DATA(self)); }

void IoMap_mark(IoMap *self) {
    // PHash_doOnKeyAndValue_(DATA(self), (ListDoCallback
    // *)IoObject_shouldMark);
    PHASH_FOREACH(DATA(self), k, v, IoObject_shouldMark(k);
                  IoObject_shouldMark(v));
}

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

IoObject *IoMap_rawAt(IoMap *self, IoSymbol *k) {
    return PHash_at_(DATA(self), k);
}

IO_METHOD(IoMap, at) {
    /*doc Map at(keyString, optionalDefaultValue)
    Returns the value for the key keyString. Returns nil if the key is absent.
    */

    IoSymbol *k = IoMessage_locals_symbolArgAt_(m, locals, 0);
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
    IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 1);
    IoMap_rawAtPut(self, k, v);
    return self;
}

IO_METHOD(IoMap, atIfAbsentPut) {
    /*doc Map atIfAbsentPut(keyString, aValue)
    If a value is present at the specified key, the value is returned.
    Otherwise, inserts/sets aValue and returns aValue.
    */

    IoSymbol *k = IoMessage_locals_symbolArgAt_(m, locals, 0);

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

IoList *IoMap_rawKeys(IoMap *self) {
    IoList *list = IoList_new(IOSTATE);
    PHASH_FOREACH(DATA(self), k, v, IoList_rawAppend_(list, k));
    return list;
}

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
    PHash *p = DATA(self);

    IoObject *result = IONIL(self);
    IoMessage_foreachArgs(m, self, &keyName, &valueName, &doMessage);
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
