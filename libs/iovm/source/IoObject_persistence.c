/*#io
Object ioDoc(

*/

#include "IoObject_persistence.h"
#include "IoObject.h"
#include "IoState.h"
#include "IoCFunction.h"
#include "PHash.h"

void IoObject_writeToStream_(IoObject *self, BStream *stream)
{
    if (IoObject_tag(self)->writeToStreamFunc)
    {
        (*(IoObject_tag(self)->writeToStreamFunc))(self, stream);
    }

    IoObject_writeStream_(self, stream);
}

IoObject *IoObject_allocFromStore_stream_(IoObject *self, BStream *stream)
{
    if (IoObject_tag(self)->allocFromStreamFunc)
    {
        return (*(IoObject_tag(self)->allocFromStreamFunc))(self, stream);
    }

    return self;
}

int IoObject_nonCFunctionSlotCount(IoObject *self)
{
    int count = 0;
    PHASH_FOREACH(IoObject_slots(self), key, value, count += !ISCFUNCTION(value); );
    return count;
}

void IoObject_writeProtosToStore_stream_(IoObject *self, BStream *stream)
{
    int i, protosCount = IoObject_rawProtosCount(self);

    BStream_writeTaggedInt32_(stream, protosCount);

    for (i = 0; i < protosCount; i ++)
    {
        IoObject *proto = IoObject_protoAt_(self, i);
        BStream_writeTaggedInt32_(stream, IoObject_pid(proto));
    }
}

void IoObject_writeStream_(IoObject *self, BStream *stream)
{
    IoObject_writeProtosToStore_stream_(self, stream);

    if (IoObject_ownsSlots(self))
    {
        BStream_writeTaggedInt32_(stream, IoObject_nonCFunctionSlotCount(self));

        if (IoObject_slots(self))
        {
            PHASH_FOREACH(IoObject_slots(self), key, value,
                if (!ISCFUNCTION(value))
                {
                    BStream_writeTaggedInt32_(stream, IoObject_pid(key));
                    BStream_writeTaggedInt32_(stream, IoObject_pid(value));
                }
            );
        }
    }
}

void IoObject_readProtosFromStore_stream_(IoObject *self, BStream *stream)
{
    int i, max = BStream_readTaggedInt32(stream);

    IoObject_rawRemoveAllProtos(self);

    for (i = 0; i < max; i ++)
    {
        int protoId = BStream_readTaggedInt32(stream);
        IoObject *proto = IoState_objectWithPid_(IOSTATE, protoId);
        IoObject_rawAppendProto_(self, proto);
    }
}

void IoObject_readSlotsFromStore_stream_(IoObject *self, BStream *stream)
{
    int k, v;
    IoSymbol *key;
    IoObject *value;
    int i, max;

    IoObject_readProtosFromStore_stream_(self, stream);

    max = BStream_readTaggedInt32(stream);

    for (i = 0; i < max; i ++)
    {
        k = BStream_readTaggedInt32(stream);
        v = BStream_readTaggedInt32(stream);

        #ifdef IO_STORE_DEBUG
            printf("%s%" PID_FORMAT " load slot %i, %i\n",
                   IoObject_name(self), IoObject_persistentId(self), k, v);
        #endif

        key   = IoState_objectWithPid_(IOSTATE, k);
        value = IoState_objectWithPid_(IOSTATE, v);
        IoObject_setSlot_to_(self, key, value);
    }
}
