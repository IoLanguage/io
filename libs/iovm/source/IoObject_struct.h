// metadoc Object copyright Steve Dekorte 2002
// metadoc Object license BSD revised

#ifndef OBJECT_STRUCT_DEFINED
#define OBJECT_STRUCT_DEFINED 1

#include "Common.h"
#include "PHash_struct.h"
#include "BStream.h"
#include "IoTag.h"
#include "Collector.h"

#ifdef __cplusplus
extern "C" {
#endif

// Shortcut for method definitons in C code
#define IO_METHOD(CLASS, NAME)                                                 \
    IoObject *CLASS##_##NAME(CLASS *self, IoObject *locals, IoMessage *m)

typedef struct CollectorMarker IoObject;
// typedef struct IoObjectData IoObjectData;
/*
#ifndef CollectorObjectReference_DEFINED
        #define CollectorObjectReference_DEFINED 1
        typedef struct IoObjectData CollectorObjectReference;
#endif
*/

#define IOOBJECT_PERSISTENCE 1

struct IoObjectData {
    unsigned int markerCount;

    union {
        void *ptr;
        double d;
        uint32_t ui32;
    } data; // 8 bytes (9+8 = 17)

    // 4*4 = 16 bytes (17+16 = 33)
    IoTag *tag;
    PHash *slots;
    List *listeners;
    IoObject **protos;

    unsigned int hasDoneLookup : 1; // used to avoid slot lookup loops
    unsigned int
        isActivatable : 1; // if true, upon activation, call activate slot
    unsigned int
        isDirty : 1; // set to true when the object changes its storable state

    // optimizations

    unsigned int ownsSlots : 1; // if true, io_free slots hash table when
                                // io_freeing object
    unsigned int
        isSymbol : 1; // true if the object is a unqiue sequence - a symbol
    unsigned int isLocals : 1;     // true if the Object is a locals object
    unsigned int isReferenced : 1; // 1 bit ref counter
    unsigned int sentWillFree : 1; // 1 is sent willFree mes
    // unsigned int isPObject : 1;        // check before type tests?
};

#define IoObject_deref(self)                                                   \
    ((IoObjectData *)(((CollectorMarker *)(self))->object))
#define IoObject_markerCount(self) IoObject_deref(self)->markerCount
#define IoObject_markerCount_(self, v) IoObject_deref(self)->markerCount = (v);
#define IoObject_incrementMarkerCount(self) IoObject_deref(self)->markerCount++;
#define IoObject_decrementMarkerCount(self) IoObject_deref(self)->markerCount--;

#define IoObject_tag_(self, t) IoObject_deref(self)->tag = t;
#define IoObject_tag(self) (IoObject_deref(self)->tag)

#define IoObject_state(self) ((IoState *)(IoObject_tag(self)->state))
#define IOSTATE ((IoState *)(IoObject_tag(self)->state))
//#define IOSTATE IoObject_state(state)

#define IoObject_dataPointer(self) IoObject_deref(self)->data.ptr
#define IoObject_setDataPointer_(self, v)                                      \
    IoObject_deref(self)->data.ptr = (void *)(v);

#define IoObject_dataDouble(self) IoObject_deref(self)->data.d
#define IoObject_setDataDouble_(self, v)                                       \
    IoObject_deref(self)->data.d = (double)(v);

#define IoObject_dataUint32(self) IoObject_deref(self)->data.ui32
#define IoObject_setDataUint32_(self, v)                                       \
    IoObject_deref(self)->data.ui32 = (uint32_t)(v);

#define IoObject_isActivatable_(self, b)                                       \
    IoObject_deref(self)->isActivatable = b;
#define IoObject_isActivatable(self) (IoObject_deref(self)->isActivatable)

#define IoObject_ownsSlots_(self, b) IoObject_deref(self)->ownsSlots = b;
#define IoObject_ownsSlots(self) (IoObject_deref(self)->ownsSlots)

#define IoObject_slots_(self, v) IoObject_deref(self)->slots = v;
#define IoObject_slots(self) (IoObject_deref(self)->slots)

#define IoObject_protos_(self, v) IoObject_deref(self)->protos = v;
#define IoObject_protos(self) (IoObject_deref(self)->protos)

#define IoObject_firstProto(self) IoObject_protos(self)[0]
#define IoObject_protoAt_(self, i) IoObject_protos(self)[i]
#define IoObject_protoAtPut_(self, i, v) IoObject_protos(self)[i] = v;

#define IoObject_listeners_(self, v) IoObject_deref(self)->listeners = v;
#define IoObject_listeners(self) (IoObject_deref(self)->listeners)

#define IoObject_hasDoneLookup_(self, v)                                       \
    IoObject_deref(self)->hasDoneLookup = v;
#define IoObject_hasDoneLookup(self) (IoObject_deref(self)->hasDoneLookup)

#define IoObject_isSymbol_(self, v) IoObject_deref(self)->isSymbol = v;
#define IoObject_isSymbol(self) (IoObject_deref(self)->isSymbol)

#ifdef IOOBJECT_PERSISTENCE
#define IoObject_isDirty_(self, v) IoObject_deref(self)->isDirty = v;
#define IoObject_isDirty(self) (IoObject_deref(self)->isDirty)
#else
#define IoObject_isDirty_(self, v)
#define IoObject_isDirty(self) 0
#endif

#define IoObject_sentWillFree_(self, v) IoObject_deref(self)->sentWillFree = v;
#define IoObject_sentWillFree(self) (IoObject_deref(self)->sentWillFree)

#define IoObject_isLocals_(self, v) IoObject_deref(self)->isLocals = v;
#define IoObject_isLocals(self) (IoObject_deref(self)->isLocals)

#ifdef IO_BLOCK_LOCALS_RECYCLING
#define IoObject_isReferenced_(self, v) IoObject_deref(self)->isReferenced = v;
#define IoObject_isReferenced(self) (IoObject_deref(self)->isReferenced)
#else
#define IoObject_isReferenced_(self, v)
#define IoObject_isReferenced(self) 0
#endif

typedef IoObject *(IoMethodFunc)(IoObject *, IoObject *, IoObject *);

typedef struct {
    const char *name;
    IoMethodFunc *func;
} IoMethodTable;

#ifdef __cplusplus
}
#endif
#endif
