
// metadoc Object category Core
// metadoc Object copyright Steve Dekorte 2002
// metadoc Object license BSD revised
/*metadoc Object description
An Object is a key/value dictionary with string keys and values of any type.
The prototype Object contains a clone slot that is a CFunction that creates new
objects. When cloned, an Object will call its init slot (with no arguments).
*/

/*cmetadoc Object description
C implementation of the root Object prototype — the ancestor of every
Io value. An IoObject is a CollectorMarker whose `object` field points
to an IoObjectData carrying the tag (vtable), a PHash of slots, an
inline-or-heap-allocated protos array, and a small data union used by
primitive subtypes. This file owns allocation (IoObject_justAlloc /
_alloc / _proto), cloning (raw / primitive / IOCLONE with init), proto
chain manipulation (rawAppendProto_, rawPrependProto_, rawRemoveProto_
with inline-or-realloc'd storage), slot read/write, lookup loop
detection, the whole Io-visible method table installed by
IoObject_protoFinish, and the localsProto variant used for block locals.
Most of the `IO_METHOD(IoObject, ...)` functions here already carry
doc blocks and are part of the Io-visible API; only the internal
C helpers (raw*, alloc/dealloc, activateFunc, lookup primitives) are
annotated below.
*/

#include "IoState.h"
#define IOOBJECT_C
#include "IoObject.h"
#undef IOOBJECT_C
#include "IoCoroutine.h"
#ifdef IO_CALLCC
#include "IoContinuation.h"
#endif
#include "IoEvalFrame.h"
#include "IoTag.h"
#include "IoCFunction.h"
#include "IoSeq.h"
#include "IoNumber.h"
#include "IoMessage.h"
#include "IoMessage_parser.h"
#include "IoCFunction.h"
#include "IoBlock.h"
#include "IoList.h"
#include "IoObject.h"
#include "IoFile.h"
#include "IoSeq.h"
#include <string.h>
#include <stddef.h>

static const char *protoId = "Object";

IoObject *IoObject_activateFunc(IoObject *self, IoObject *target,
                                IoObject *locals, IoMessage *m,
                                IoObject *slotContext);

/*cdoc Object IoObject_newTag(state)
Builds the root Object tag. Only cloneFunc is wired up — activateFunc
is installed lazily by setIsActivatableMethod when an object first
opts in. No freeFunc is set either: generic Object slots are freed
through IoObject_dealloc's own path, not through a per-tag hook.
*/
IoTag *IoObject_newTag(void *state) {
    IoTag *tag = IoTag_newWithName_(protoId);
    IoTag_state_(tag, state);
    IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoObject_rawClone);
    IoTag_activateFunc_(tag,
                        (IoTagActivateFunc *)NULL); // IoObject_activateFunc;
    return tag;
}

/*cdoc Object IoObject_justAlloc(state)
Allocates an IoObject marker plus its IoObjectData in a single heap
block, with space for a 2-pointer inline protos array placed right
after the data struct. This co-allocation avoids a separate small
allocation per object and lets the GC free the whole lot with one
io_free — the protos array is only promoted to a heap buffer when
IoObject_rawAppendProto_ / _rawPrependProto_ outgrow the inline slots.
*/
IoObject *IoObject_justAlloc(IoState *state) {
    IoObject *child = Collector_newMarker(state->collector);
    // Allocate IoObjectData + 2-pointer protos array in a single block.
    // The protos array is placed immediately after the IoObjectData struct.
    IoObjectData *data =
        io_calloc(1, sizeof(IoObjectData) + 2 * sizeof(IoObject *));
    CollectorMarker_setObject_(child, data);
    IoObject_protos_(child, (IoObject **)(data + 1));
    return child;
}

/*cdoc Object IoObject_alloc(self)
Primary allocator used by the clone paths. Pops from the IoState
recycled-object pool when IOSTATE_RECYCLING_ON is defined; otherwise
falls through to IoObject_justAlloc. Resets markerCount so the new
object does not carry over any become-tracking from its previous life
in the pool.
*/
IoObject *IoObject_alloc(IoObject *self) {
    IoObject *child;

#ifdef IOSTATE_RECYCLING_ON
    child = List_pop(IOSTATE->recycledObjects);
    if (!child)
#endif
    {
        child = IoObject_justAlloc(IOSTATE);
    }

    IoObject_markerCount_(child, 0);

    return child;
}

/*cdoc Object IoObject_proto(state)
Creates the root Object proto — the first object ever allocated in a
fresh IoState. Gives it its own fresh slots hash (ownsSlots = 1, unlike
ordinary clones which share slots with their proto until written) and
registers it on the state. Method table installation is deferred to
IoObject_protoFinish so dependent protos can be registered first.
*/
IoObject *IoObject_proto(void *state) {
    IoObject *self = IoObject_justAlloc(state);

    IoObject_tag_(self, IoObject_newTag(state));

    IoObject_slots_(self, PHash_new());
    IoObject_ownsSlots_(self, 1);
    // IoObject_state_(self, state);
    IoState_registerProtoWithId_((IoState *)state, self, protoId);
    return self;
}

IO_METHOD(IoObject, protoOwnsSlots) {
    /*doc Object ownsSlots
    A debug method.
    */
    return IOBOOL(self, IoObject_ownsSlots(self));
}

/*cdoc Object IoObject_memorySize(self)
Best-effort byte count of what the object owns: sizeof(IoObject) plus
the owned slots hash if any. Primitive-specific data attached via
IoObject_dataPointer is NOT counted here — that used to be summed
through a per-tag memorySizeFunc but has been disabled; primitives
report their own size through their own introspection methods.
*/
size_t IoObject_memorySize(IoObject *self) {
    // return (IoObject_tag(self)->memorySizeFunc) ?
    // (IoObject_tag(self)->memorySizeFunc)(self) : 0;
    size_t size = sizeof(IoObject);

    if (IoObject_ownsSlots(self))
        size += PHash_memorySize(IoObject_slots(self));

    if (!ISNUMBER(self)) {
        //		if(IoObject_dataPointer(self)) size +=
        // io_memsize(IoObject_dataPointer(self));
    }

    return size;
}

/*cdoc Object IoObject_protoFinish(state)
Second-phase Object proto setup: installs the full Io-visible method
table on the already-registered proto. Split from IoObject_proto so
that cross-referencing primitives (CFunction, Number, Sequence, etc.)
can be registered first — the method table wraps each C function as an
IoCFunction, which requires those primitives to exist. Uses the
tagless method path so slot lookups do not short-circuit on a
class-matched tag.
*/
IoObject *IoObject_protoFinish(void *state) {
    IoMethodTable methodTable[] = {
        {"clone", IoObject_clone},
        {"cloneWithoutInit", IoObject_cloneWithoutInit},
        {"shallowCopy", IoObject_shallowCopy},
        {"write", IoObject_protoWrite},
        {"writeln", IoObject_protoWriteLn},
        {"type", IoObject_type},

        // logic

        {"compare", IoObject_protoCompare},
        {"<", IoObject_isLessThan_},
        {">", IoObject_isGreaterThan_},
        {">=", IoObject_isGreaterThanOrEqualTo_},
        {"<=", IoObject_isLessThanOrEqualTo_},
        {"asBoolean", IoObject_asBoolean},

        // comparison

        {"isIdenticalTo", IoObject_isIdenticalTo},
        {"==", IoObject_equals},
        {"!=", IoObject_notEquals},

        // introspection

        //{"self", IoObject_self},
        {"setSlot", IoObject_protoSet_to_},
        {"setSlotWithType", IoObject_protoSetSlotWithType},
        {"updateSlot", IoObject_protoUpdateSlot_to_},
        {"getSlot", IoObject_protoGetSlot_},
        {"getLocalSlot", IoObject_protoGetLocalSlot_},
        {"hasLocalSlot", IoObject_protoHasLocalSlot},
        {"hasProto", IoObject_protoHasProto_},
        {"removeSlot", IoObject_protoRemoveSlot},
        {"removeAllSlots", IoObject_protoRemoveAllSlots},
        {"slotNames", IoObject_protoSlotNames},
        {"slotValues", IoObject_protoSlotValues},

        // method invocation

        {"perform", IoObject_protoPerform},
        {"performWithArgList", IoObject_protoPerformWithArgList},
        {"ancestorWithSlot", IoObject_ancestorWithSlot},
        {"contextWithSlot", IoObject_contextWithSlot},

        // control

        {"block", IoObject_block},
        {"method", IoBlock_method},
        {"for", IoObject_for},
        {"if", IoObject_if},
        {"", IoObject_evalArg},
        //{"truthValueOfArg", IoObject_truthValueOfArg},
        {"evalArg", IoObject_evalArg},
        {"evalArgAndReturnSelf", IoObject_evalArgAndReturnSelf},
        {"evalArgAndReturnNil", IoObject_evalArgAndReturnNil},

        {"return", IoObject_return},
        {"returnIfNonNil", IoObject_returnIfNonNil},
        {"loop", IoObject_loop},
        {"while", IoObject_while},
        {"break", IoObject_break},
        {"continue", IoObject_continue},
        {"stopStatus", IoObject_stopStatus},
#ifdef IO_CALLCC
        {"callcc", IoObject_callcc},
#endif

        // utility

        {"print", IoObject_lobbyPrint},
        {"do", IoObject_do},
        {"lexicalDo", IoObject_lexicalDo},
        {"message", IoObject_message},
        {"doMessage", IoObject_doMessage},
        {"doString", IoObject_doString},
        {"doFile", IoObject_doFile},

        // reflection

        {"uniqueId", IoObject_uniqueId},

        //{"compact", IoObject_compactMethod},

        {"init", IoObject_self},

        // enumeration

        {"foreachSlot", IoObject_foreachSlot},
        {"-", IoObject_subtract},

        {"thisContext", IoObject_self},
        {"thisMessage", IoObject_thisMessage},
        {"thisLocalContext", IoObject_locals},

        // protos

        {"setProto", IoObject_setProto},
        {"setProtos", IoObject_setProtos},
        {"appendProto", IoObject_appendProto},
        {"prependProto", IoObject_prependProto},
        {"removeProto", IoObject_removeProto},
        {"removeAllProtos", IoObject_removeAllProtos},
        {"protos", IoObject_protosMethod},
        {"proto", IoObject_objectProto},
        {"setIsActivatable", IoObject_setIsActivatableMethod},
        {"isActivatable", IoObject_isActivatableMethod},
        {"argIsActivationRecord", IoObject_argIsActivationRecord},
        {"argIsCall", IoObject_argIsCall},
        {"become", IoObject_become},

        {"ownsSlots", IoObject_protoOwnsSlots}, // a debug method
        {"memorySize", IoObject_memorySizeMethod},

        {"hasDirtySlot", IoObject_hasDirtySlot_},
        {"markClean", IoObject_markClean},

        {NULL, NULL},
    };

    IoObject *self = IoState_protoWithId_((IoState *)state, protoId);

    IoObject_addTaglessMethodTable_(self, methodTable);
    return self;
}

/*cdoc Object IoObject_localsProto(state)
Builds the prototype used as the `locals` object in every block/method
activation. Starts from a clone of the root Object's slots, drops the
proto chain so slot lookups don't bleed into the lobby, and installs
locals-specific slot semantics (setSlot / updateSlot / thisLocalContext
plus a `forward` slot that delegates to `self`). Frames set up by the
iterative evaluator and the recursive fallback both clone this proto.
*/
IoObject *IoObject_localsProto(void *state) {
    IoObject *self = IoObject_new(state);

    IoObject_createSlotsIfNeeded(self);
    PHash_copy_(IoObject_slots(self),
                IoObject_slots(IoObject_firstProto(self)));

    IoObject_rawRemoveAllProtos(self);

    // Locals handles := and =
    IoObject_addMethod_(self, IOSYMBOL("setSlot"), IoObject_protoSet_to_);
    IoObject_addMethod_(self, IOSYMBOL("setSlotWithType"),
                        IoObject_protoSetSlotWithType);
    IoObject_addMethod_(self, IOSYMBOL("updateSlot"),
                        IoObject_localsUpdateSlot);
    IoObject_addMethod_(self, IOSYMBOL("thisLocalContext"), IoObject_locals);

    // Everything else is forwarded to self
    IoObject_addMethod_(self, IOSYMBOL("forward"), IoObject_localsForward);

    return self;
}

/*cdoc Object IoObject_addMethod_(self, slotName, fp)
Wraps a C function pointer as an IoCFunction bound to self's tag and
installs it under slotName. The tag binding makes the wrapped function
fail fast if it is ever invoked on a receiver of the wrong class —
useful for primitive methods that depend on the data union layout.
*/
IoObject *IoObject_addMethod_(IoObject *self, IoSymbol *slotName,
                              IoMethodFunc *fp) {
    IoTag *t = IoObject_tag(self);
    IoCFunction *f;

    f = IoCFunction_newWithFunctionPointer_tag_name_(
        IOSTATE, (IoUserFunction *)fp, t, CSTRING(slotName));
    IoObject_setSlot_to_(self, slotName, f);
    return f;
}

/*cdoc Object IoObject_addMethodTable_(self, methodTable)
Bulk-installs a {name, fp, ...} array terminated by {NULL, NULL}. The
convention every primitive uses to register its Io-visible API.
*/
void IoObject_addMethodTable_(IoObject *self, IoMethodTable *methodTable) {
    IoMethodTable *entry = methodTable;

    while (entry->name) {
        IoObject_addMethod_(self, IOSYMBOL(entry->name), entry->func);
        entry++;
    }
}

/*cdoc Object IoObject_addTaglessMethod_(self, slotName, fp)
Variant of IoObject_addMethod_ that binds the wrapper to no tag, so
the method accepts any receiver. Used for root Object methods that
must work uniformly across every subtype (e.g. clone, setSlot, type).
*/
IoObject *IoObject_addTaglessMethod_(IoObject *self, IoSymbol *slotName,
                                     IoMethodFunc *fp) {
    IoCFunction *f;

    f = IoCFunction_newWithFunctionPointer_tag_name_(
        IOSTATE, (IoUserFunction *)fp, NULL, CSTRING(slotName));
    IoObject_setSlot_to_(self, slotName, f);
    return f;
}

/*cdoc Object IoObject_addTaglessMethodTable_(self, methodTable)
Bulk form of addTaglessMethod_ used by IoObject_protoFinish to install
the root method table without locking each method to Object's tag.
*/
void IoObject_addTaglessMethodTable_(IoObject *self,
                                     IoMethodTable *methodTable) {
    IoMethodTable *entry = methodTable;

    while (entry->name) {
        IoObject_addTaglessMethod_(self, IOSYMBOL(entry->name), entry->func);
        entry++;
    }
}

/*cdoc Object IoObject_new(state)
Convenience: fetch the registered Object proto and clone it via the
full IOCLONE path (collector pause + optional init). The standard
way C code constructs a bare IoObject.
*/
IoObject *IoObject_new(void *state) {
    IoObject *proto = IoState_protoWithId_((IoState *)state, protoId);
    return IOCLONE(proto);
}

/*cdoc Object IoObject_justClone(self)
Calls the tag's cloneFunc directly, bypassing IOCLONE's collector
pause and addValueIfNecessary bookkeeping. Used when the caller is
already holding the object reachable via some other path and can
afford to skip GC protection.
*/
IoObject *IoObject_justClone(IoObject *self) {
    return (IoObject_tag(self)->cloneFunc)(self);
}

void IoObject_createSlots(IoObject *self) {
    IoObject_slots_(self, PHash_new());
    IoObject_ownsSlots_(self, 1);
}

/*cdoc Object IoObject_freeData(self)
Releases the primitive subtype's data pointer. Prefers the tag's
freeFunc when one is registered (tag-specific cleanup for Seq, List,
Map, Block, etc.); otherwise io_frees the raw buffer. Always nulls
the data pointer afterward so a subsequent dealloc does not double-free.
*/
// inline
void IoObject_freeData(IoObject *self) {
    IoTagFreeFunc *func = IoTag_freeFunc(IoObject_tag(self));

    if (func) {
        // if(func == free)
        {
            // printf("Tag func is free\n");
            // if (IoObject_name(self)) printf("free %s\n",
            // IoObject_name(self));
        }

        (*func)(self);
    } else if (IoObject_dataPointer(self)) {
        io_free(IoObject_dataPointer(self));
    }

    IoObject_setDataPointer_(self, NULL);
}

/*cdoc Object IoObject_setProtoTo_(self, proto)
Sets self's single proto and, if self has no slots yet, shares proto's
slots hash with ownsSlots = 0. This is the copy-on-write trick behind
Io's cloning: a fresh clone reads its proto's slots until the first
write, which triggers creation of its own hash (see
IoObject_createSlotsIfNeeded).
*/
// inline
void IoObject_setProtoTo_(IoObject *self, IoObject *proto) {
    IoObject_rawSetProto_(self, proto);

    if (!IoObject_slots(self)) {
        IoObject_slots_(self, IoObject_slots(proto));
        IoObject_ownsSlots_(self, 0); // should be redundant
    }
}

/*cdoc Object IoObject_rawClone(proto)
Default cloneFunc for the root Object tag: allocate a fresh marker,
copy proto's tag and isActivatable flag, install proto as the single
proto (sharing its slots until first write), and mark dirty so
persistence and incremental GC notice the new object. Primitive
subtypes install their own cloneFunc via IoXxx_rawClone.
*/
IoObject *IoObject_rawClone(IoObject *proto) {
    IoObject *self = IoObject_alloc(proto);
    IoObject_tag_(self, IoObject_tag(proto));
    /*
    {
            IoObject **protos = IoObject_protos(self);
            protos[0] = proto;
    }
    */
    IoObject_setProtoTo_(self, proto);
    IoObject_isActivatable_(self, IoObject_isActivatable(proto));

    // IoObject_protos(self)[0] = proto;
    // IoObject_setDataPointer_(self, IoObject_dataPointer(proto)); // is this
    // right?
    IoObject_isDirty_(self, 1);
    return self;
}

/*cdoc Object IoObject_rawClonePrimitive(proto)
Clone helper used by primitive subtypes (Number, Seq, List, Block,
Message, Call, EvalFrame, Continuation, ...). Same shape as
IoObject_rawClone but explicitly sets dataPointer to NULL so the
primitive's own rawClone can install a fresh data payload without
accidentally aliasing the proto's.
*/
IoObject *IoObject_rawClonePrimitive(IoObject *proto) {
    IoObject *self = IoObject_alloc(proto);
    IoObject_tag_(self, IoObject_tag(proto));
    IoObject_setProtoTo_(self, proto);
    IoObject_setDataPointer_(self, NULL);
    IoObject_isActivatable_(self, IoObject_isActivatable(proto));
    IoObject_isDirty_(self, 1);
    return self;
}

// protos ---------------------------------------------

/*cdoc Object IoObject_rawPrintProtos(self)
Debug helper that prints the proto chain with index numbers. Used when
tracing lookup ordering; not reachable from Io code.
*/
void IoObject_rawPrintProtos(IoObject *self) {
    int count = 0;

    IOOBJECT_FOREACHPROTO(
        self, proto, printf("%i : %p\n", count, (void *)(proto)); count++;);

    printf("\n");
}

int IoObject_hasProtos(IoObject *self) {
    return (IoObject_firstProto(self) != NULL);
}

/*cdoc Object IoObject_rawProtosCount(self)
Walks the NULL-terminated protos array and returns the number of
non-NULL entries. Cheap — typical Io objects have 1-2 protos.
*/
int IoObject_rawProtosCount(IoObject *self) {
    int count = 0;
    IOOBJECT_FOREACHPROTO(self, proto, if (proto) count++);
    return count;
}

/*cdoc Object IoObject_rawAppendProto_(self, p)
Appends p to the proto list. The subtlety is that the initial protos
array lives inline inside the IoObjectData block (see
IoObject_justAlloc), so it cannot simply be realloc'd — this function
detects the inline case by pointer comparison and, if needed,
allocates a fresh heap buffer and copies the old entries before
extending. All proto-mutation paths go through this or its sibling
helpers so the inline/heap discrimination stays in one place.
*/
void IoObject_rawAppendProto_(IoObject *self, IoObject *p) {
    int count = IoObject_rawProtosCount(self);
    IoObject **oldProtos = IoObject_protos(self);
    IoObjectData *data = IoObject_deref(self);

    if ((void *)oldProtos == (void *)(data + 1)) {
        // Protos are inline (part of data block) - can't realloc
        IoObject **newProtos =
            (IoObject **)io_calloc(count + 2, sizeof(IoObject *));
        memcpy(newProtos, oldProtos, (count + 1) * sizeof(IoObject *));
        IoObject_protos_(self, newProtos);
    } else {
        IoObject_protos_(self, io_realloc(oldProtos,
                                          (count + 2) * sizeof(IoObject *)));
    }
    IoObject_protos(self)[count] = IOREF(p);
    IoObject_protos(self)[count + 1] = NULL;
}

/*cdoc Object IoObject_rawPrependProto_(self, p)
Prepends p, so it takes precedence in slot lookup. Like rawAppendProto_
it must handle the inline-vs-heap array case separately, then memmove
the existing entries up by one slot.
*/
void IoObject_rawPrependProto_(IoObject *self, IoObject *p) {
    int count = IoObject_rawProtosCount(self);
    int oldSize = (count + 1) * sizeof(IoObject *);
    int newSize = oldSize + sizeof(IoObject *);
    IoObject **oldProtos = IoObject_protos(self);
    IoObjectData *data = IoObject_deref(self);

    if ((void *)oldProtos == (void *)(data + 1)) {
        // Protos are inline - allocate new array
        IoObject **newProtos =
            (IoObject **)io_calloc(1, newSize);
        memcpy(newProtos + 1, oldProtos, oldSize);
        IoObject_protos_(self, newProtos);
    } else {
        IoObject_protos_(self, io_realloc(oldProtos, newSize));
        void *src = IoObject_protos(self);
        void *dst = IoObject_protos(self) + 1;
        memmove(dst, src, oldSize);
    }

    IoObject_protoAtPut_(self, 0, IOREF(p));
}

/*cdoc Object IoObject_rawRemoveProto_(self, p)
Removes every occurrence of p from the proto array by memmove-compacting
over each match. The storage block is not freed here — the array may
stay oversized after removal, which is fine since proto lists are small.
*/
void IoObject_rawRemoveProto_(IoObject *self, IoObject *p) {
    IoObject **proto = IoObject_protos(self);
    int count = IoObject_rawProtosCount(self);
    int index = 0;

    while (*proto) {
        if (*proto == p) {
            memmove(proto, proto + 1, (count - index) * sizeof(IoObject *));
        } else {
            proto++;
        }

        index++;
    }
}

/*
#include <assert.h>

void IoObject_testProtosCode(IoObject *self)
{
        IoObject *o1 = (IoObject *)0x1;
        IoObject *o2 = (IoObject *)0x2;
        int c;

        IoObject_rawRemoveAllProtos(self);
        c = IoObject_rawProtosCount(self);
        assert(c == 0);

        IoObject_rawAppendProto_(self, o1);
        assert(IoObject_protoAt_(self, 0) == o1);
        assert(IoObject_protoAt_(self, 1) == NULL);
        assert(IoObject_rawProtosCount(self) == 1);

        IoObject_rawPrependProto_(self, (IoObject *)0x2);
        assert(IoObject_rawProtosCount(self) == 2);
        assert(IoObject_protoAt_(self, 0) == o2);
        assert(IoObject_protoAt_(self, 1) == o1);
        assert(IoObject_protoAt_(self, 2) == NULL);

        IoObject_rawRemoveAllProtos(self);
        c = IoObject_rawProtosCount(self);
        assert(c == 0);
}
*/

/*cdoc Object IoObject_rawSetProto_(self, proto)
Replaces the entire proto list with a single entry. IOREF participates
in the collector write barrier so proto stays marked even before the
next sweep.
*/
void IoObject_rawSetProto_(IoObject *self, IoObject *proto) {
    IoObject_rawRemoveAllProtos(self);
    IoObject_protos(self)[0] = IOREF(proto);
}

IO_METHOD(IoObject, objectProto) {
    /*doc Object proto
    Same as; method(self protos first)
    */

    IoObject *proto = IoObject_firstProto(self);
    return proto ? proto : IONIL(self);
}

IO_METHOD(IoObject, setProto) {
    /*doc Object setProto(anObject)
    Sets the first proto of the receiver to anObject, replacing the
    current one, if any. Returns self.
    */

    IoObject_rawSetProto_(self, IoMessage_locals_valueArgAt_(m, locals, 0));
    return self;
}

IO_METHOD(IoObject, appendProto) {
    /*doc Object appendProto(anObject)
    Appends anObject to the receiver's proto list. Returns self.
    */

    IoObject *proto = IoMessage_locals_valueArgAt_(m, locals, 0);
    IoObject_rawAppendProto_(self, proto);
    IOSTATE->slotVersion++;
    return self;
}

IO_METHOD(IoObject, prependProto) {
    /*doc Object prependProto(anObject)
    Prepends anObject to the receiver's proto list. Returns self.
    */

    IoObject *proto = IoMessage_locals_valueArgAt_(m, locals, 0);
    IoObject_rawPrependProto_(self, proto);
    IOSTATE->slotVersion++;
    return self;
}

IO_METHOD(IoObject, removeProto) {
    /*doc Object removeProto(anObject)
    Removes anObject from the receiver's proto list if it
    is present. Returns self.
    */

    IoObject *proto = IoMessage_locals_valueArgAt_(m, locals, 0);
    IoObject_rawRemoveProto_(self, proto);
    IOSTATE->slotVersion++;
    return self;
}

IO_METHOD(IoObject, removeAllProtos) {
    /*doc Object removeAllProtos
    Removes all of the receiver's protos. Returns self.
    */

    IoObject_rawRemoveAllProtos(self);
    IOSTATE->slotVersion++;
    return self;
}

IO_METHOD(IoObject, setProtos) {
    /*doc Object setProtos(aList)
    Replaces the receiver's protos with a copy of aList. Returns self.
    */

    IoList *ioList = IoMessage_locals_listArgAt_(m, locals, 0);
    if (IOSTATE->errorRaised) return IONIL(self);
    IoObject_rawRemoveAllProtos(self);
    LIST_FOREACH(IoList_rawList(ioList), i, v,
                 IoObject_rawAppendProto_(self, (IoObject *)v));
    IOSTATE->slotVersion++;
    return self;
}

IO_METHOD(IoObject, protosMethod) {
    /*doc Object protos
    Returns a copy of the receiver's protos list.
    */

    IoList *ioList = IoList_new(IOSTATE);
    List *list = IoList_rawList(ioList);
    IOOBJECT_FOREACHPROTO(self, proto, List_append_(list, proto));
    return ioList;
}

// --------------------------------------------------------

/*cdoc Object IoObject_freeSlots(self)
Releases the PHash slots table if this object owns it (a fresh clone
shares its proto's table until first write, and must not free what
it does not own). Called from the free/recycle path. The redundant
second slots_ call guards against a future change that starts owning
shared slots.
*/
// inline
void IoObject_freeSlots(
    IoObject *self) // prepare for io_free and possibly recycle
{
    if (IoObject_ownsSlots(self)) {
        PHash_free(IoObject_slots(self));
        IoObject_slots_(self, NULL);
        IoObject_ownsSlots_(self, 0);
    }

    IoObject_slots_(self, NULL);
}

/*cdoc Object IoObject_willFree(self)
Pre-free hook used only under COLLECTOR_USE_REFCOUNT builds to
decrement reference counts on every slot value before the object's
data is reclaimed. With the tri-color collector (default) this is a
no-op.
*/
void IoObject_willFree(IoObject *self) {
#ifdef COLLECTOR_USE_REFCOUNT
    if (IoObject_ownsSlots(self)) {
        PHASH_FOREACH(IoObject_slots(self), k, v,
            (void)k;
            Collector_value_removingRefTo_(IOCOLLECTOR, v);
        );
    }
#else
    (void)self;
#endif
}

/*cdoc Object IoObject_free(self)
Collector freeFunc entry point. Either hands the object fully over to
IoObject_dealloc or, when IOSTATE_RECYCLING_ON is enabled and the
recycle pool has room, scrubs its flags / protos / slots and stashes
it on IOSTATE->recycledObjects for IoObject_alloc to pick up. The
recycled form keeps only the bare marker + slots hash shell.
*/
void IoObject_free(IoObject *self) // prepare for io_free and possibly recycle
{
#ifdef IOSTATE_RECYCLING_ON
    if (List_size(IOSTATE->recycledObjects) >= IOSTATE->maxRecycledObjects)
#endif
    {
        IoObject_dealloc(self);
        // CollectorMarker_free((CollectorMarker *)self);
    }
#ifdef IOSTATE_RECYCLING_ON
    else {
        // printf("recycling %p\n", (void *)self);
        IoObject_rawRemoveAllProtos(self);

#ifdef IOOBJECT_PERSISTENCE
        IoObject_isDirty_(self, 0);
#endif

        IoObject_hasDoneLookup_(self, 0);
        IoObject_isSymbol_(self, 0);
        IoObject_isLocals_(self, 0);
        IoObject_isActivatable_(self, 0);

        // if (IoObject_ownsSlots(self))
        {
            // IoObject_freeSlots(self);
            PHash_clean(IoObject_slots(self));
        }
        /*
        else
        {
                IoObject_slots_(self, NULL);
        }
        */

        List_append_(IOSTATE->recycledObjects, self);
    }
#endif
}

/*cdoc Object IoObject_dealloc(self)
Truly frees the object: runs listener notifications, tag-specific
free (via IoObject_freeData), owned slots, and then the IoObjectData
block itself. Numbers with an inline protos array are instead pushed
onto state->numberDataFreeList (capped at NUMBER_DATA_POOL_MAX) so
hot numeric code can recycle the data+protos block without hitting
the general allocator. Respects markerCount so become-aliased objects
don't free data that another marker still points at.
*/
void IoObject_dealloc(IoObject *self) // really io_free it
{
    if (IoObject_markerCount(self) == 0) {
        if (IoObject_listeners(self)) {
            LIST_FOREACH(
                IoObject_listeners(self), i, v,
                IoObject_tag((IoObject *)v)->notificationFunc(v, self));
            List_free(IoObject_listeners(self));
            IoObject_listeners_(self, NULL);
        }

        IoObject_freeData(self);

        if (IoObject_ownsSlots(self)) {
            PHash_free(IoObject_slots(self));
        }

        {
            IoObjectData *objData = self->object;
            IoObject **protos = objData->protos;
            int protosInline = ((void *)protos == (void *)(objData + 1));

            // Try to recycle data+protos block for Number allocation
            IoState *st = objData->tag ? (IoState *)objData->tag->state : NULL;
            if (st && objData->tag == st->numberTag &&
                protosInline &&
                st->numberDataFreeListSize < NUMBER_DATA_POOL_MAX) {
                // Put on freelist (will be zeroed on reuse)
                objData->data.ptr = st->numberDataFreeList;
                st->numberDataFreeList = objData;
                st->numberDataFreeListSize++;
            } else {
                if (!protosInline) {
                    io_free(protos);
                }
                io_free(objData);
            }
        }
    } else {
        // printf("IoObject_decrementMarkerCount(%p)\n", (void *)self);
        IoObject_decrementMarkerCount(self)
    }
}

// ----------------------------------------------------------------

IO_METHOD(IoObject, protoCompare) {
    /*doc Object compare(anObject)
    Returns a number containing the comparison value of the target with
    anObject.
    */

    IOASSERT(IoMessage_argCount(m), "compare requires argument");

    {
        IoSymbol *other = IoMessage_locals_valueArgAt_(m, locals, 0);
        return IONUMBER(IoObject_compare(self, other));
    }
}

// slot lookups with lookup loop detection

/*cdoc Object IoObject_rawHasProto_(self, p)
Recursive proto-chain membership test with cycle detection via the
hasDoneLookup flag: the flag is set on entry and cleared on exit so a
cycle through the same object terminates without infinite recursion.
Returns 1 if p is self or is reachable through any proto.
*/
unsigned int IoObject_rawHasProto_(IoObject *self, IoObject *p) {
    if (self == p) {
        return 1;
    }

    if (IoObject_hasDoneLookup(self)) {
        return 0;
    } else {
        IoObject **proto = IoObject_protos(self);

        IoObject_hasDoneLookup_(self, 1);

        while (*proto) {
            if (IoObject_rawHasProto_(*proto, p)) {
                IoObject_hasDoneLookup_(self, 0);
                return 1;
            }

            proto++;
        }

        IoObject_hasDoneLookup_(self, 0);
        return 0;
    }
}

IO_METHOD(IoObject, protoHasProto_) {
    /*doc Object hasProto(anObject)
    Returns true if anObject is found in the proto path of the target, false
    otherwise.
    */

    IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 0);
    return IOBOOL(self, IoObject_rawHasProto_(self, v));
}

// ------------------------------------------------------

/*cdoc Object IoObject_getSlot_(self, slotName)
C-side convenience: look up a slot along the proto chain and return
ioNil rather than NULL on miss. For callers that want a raw miss
signal, use IoObject_rawGetSlot_ directly.
*/
IoObject *IoObject_getSlot_(IoObject *self, IoSymbol *slotName) {
    IoObject *v = IoObject_rawGetSlot_(self, slotName);
    return v ? v : IONIL(self);
}

/*cdoc Object IoObject_doubleGetSlot_(self, slotName)
Type-checked slot read that unwraps to a C double. Raises an Io-level
error (and returns 0) if the slot is missing or not a Number. The
error path uses IoState_error_ and relies on the caller (or the eval
loop) to check state->errorRaised.
*/
double IoObject_doubleGetSlot_(IoObject *self, IoSymbol *slotName) {
    IoObject *v = IoObject_rawGetSlot_(self, slotName);

    if (!v) {
        IoState_error_(IOSTATE, NULL, "missing slot %s in %s",
                       CSTRING(slotName), IoObject_name(self));
        return 0;
    }

    if (!ISNUMBER(v)) {
        IoState_error_(
            IOSTATE, NULL, "slot %s in %s must be a number, not a %s",
            CSTRING(slotName), IoObject_name(self), IoObject_name(v));
        return 0;
    }

    return CNUMBER(v);
}

/*cdoc Object IoObject_symbolGetSlot_(self, slotName)
Type-checked slot read expecting a Symbol. Mirrors doubleGetSlot_'s
error-on-missing-or-wrong-type pattern but returns NULL on failure so
callers can distinguish the error (after checking state->errorRaised).
*/
IoObject *IoObject_symbolGetSlot_(IoObject *self, IoSymbol *slotName) {
    IoObject *v = IoObject_rawGetSlot_(self, slotName);

    if (!v) {
        IoState_error_(IOSTATE, NULL, "missing slot %s in %s",
                       CSTRING(slotName), IoObject_name(self));
        return NULL;
    }

    if (!ISSYMBOL(v)) {
        IoState_error_(
            IOSTATE, NULL, "slot %s in %s must be a symbol, not a %s",
            CSTRING(slotName), IoObject_name(self), IoObject_name(v));
        return NULL;
    }

    return v;
}

/*cdoc Object IoObject_seqGetSlot_(self, slotName)
Type-checked slot read expecting a Sequence. Note that unlike the
other type-checked getters, this one falls through without returning
after raising errors, so the caller always gets the (possibly wrong)
value back — callers must check errorRaised and discard v on failure.
*/
IoObject *IoObject_seqGetSlot_(IoObject *self, IoSymbol *slotName) {
    IoObject *v = IoObject_rawGetSlot_(self, slotName);

    if (!v) {
        IoState_error_(IOSTATE, NULL, "missing slot %s in %s",
                       CSTRING(slotName), IoObject_name(self));
    }

    if (!ISSEQ(v)) {
        IoState_error_(
            IOSTATE, NULL, "slot %s in %s must be a sequence, not a %s",
            CSTRING(slotName), IoObject_name(self), IoObject_name(v));
    }

    return v;
}

/*cdoc Object IoObject_activateFunc(self, target, locals, m, slotContext)
Optional tag activateFunc for objects that opt into "activate on read"
(see IoObject_setIsActivatableMethod). When isActivatable is set, this
looks up the "activate" slot and activates its value with the original
target/locals/message, so the object acts like a callable. When the
flag is off it just returns self, letting the slot behave as data.
*/
IoObject *IoObject_activateFunc(IoObject *self, IoObject *target,
                                IoObject *locals, IoMessage *m,
                                IoObject *slotContext) {
    IoState *state = IOSTATE;

    if (IoObject_isActivatable(self)) {
        IoObject *context;
        IoObject *slotValue =
            IoObject_rawGetSlot_context_(self, state->activateSymbol, &context);

        if (slotValue) {
            // return IoObject_activate(slotValue, self, locals, m, context);
            return IoObject_activate(slotValue, target, locals, m, context);
        }
    }

    return self;
}

// -----------------------------------------------------------

/*cdoc Object IoObject_setSlot_to_(self, slotName, value)
C-callable setter. Thin wrapper around the inline version so non-inline
call sites (and other translation units) have a stable symbol to link
against. Handles the copy-on-write promotion of a shared slots hash
via IoObject_createSlotsIfNeeded inside inlineSetSlot_to_.
*/
void IoObject_setSlot_to_(IoObject *self, IoSymbol *slotName, IoObject *value) {
    IoObject_inlineSetSlot_to_(self, slotName, value);
}

/*cdoc Object IoObject_removeSlot_(self, slotName)
Removes a slot if present and bumps the global slotVersion counter so
inline caches (lookup caches on messages) can invalidate. Creates
slots first to keep the invariant that writes always target an owned
hash rather than a shared one.
*/
void IoObject_removeSlot_(IoObject *self, IoSymbol *slotName) {
    IoObject_createSlotsIfNeeded(self);
    PHash_removeKey_(IoObject_slots(self), slotName);
    IOSTATE->slotVersion++;
}

/*cdoc Object IoObject_rawGetSlot_target_(self, slotName, target)
Two-level slot lookup: first tries self's proto chain, and on miss
consults the object's "self" slot (if any, and different from self)
as a delegate. Writes the delegate into *target when the fallback
lookup succeeds so the caller can activate the resulting value with
the right receiver. Used by the locals-forwarding path.
*/
IoObject *IoObject_rawGetSlot_target_(IoObject *self, IoSymbol *slotName,
                                      IoObject **target) {
    IoObject *slotValue = IoObject_rawGetSlot_(self, slotName);

    if (!slotValue) {
        IoObject *selfDelegate =
            IoObject_rawGetSlot_(self, IOSTATE->selfSymbol);

        if (selfDelegate && selfDelegate != self) {
            slotValue = IoObject_rawGetSlot_(selfDelegate, slotName);

            if (slotValue) {
                *target = selfDelegate;
            }
        }
    }
    return slotValue;
}

IO_METHOD(IoObject, localsForward) {
    /*doc Object localsForward
    CFunction used by Locals prototype for forwarding.
    */

    // IoObject *selfDelegate = IoObject_rawGetSlot_(self, IOSTATE->selfSymbol);
    IoObject *selfDelegate = PHash_at_(
        IoObject_slots(self), IOSTATE->selfSymbol); // cheating a bit here

    if (selfDelegate && selfDelegate != self) {
        return IoObject_perform(selfDelegate, locals, m);
    }

    return IONIL(self);
}

// name ------------------------------------------------------

IO_METHOD(IoObject, lobbyPrint) {
    /*doc Object print
    Prints a string representation of the object. Returns Nil.
    */

    IoState *state = IOSTATE;
    const char *name = IoObject_name(self);

    IoObject_createSlotsIfNeeded(self);

    IoState_print_(state, "%s_%p do(\n", name, (void *)self, name);
    IoState_print_(state, "  appendProto(");

    {
        IoObject **proto = IoObject_protos(self);

        while (*proto) {
            IoState_print_(state, "%s_%p", name, (void *)*proto, name);
            proto++;

            if (*proto) {
                IoState_print_(state, ", ");
            }
        }
    }

    IoState_print_(state, ")\n");

    return state->ioNil;
}

/*cdoc Object IoObject_memorySizeFunc(self)
Tag memorySizeFunc candidate returning data+slots bytes. Currently
unused — IoObject_memorySize walks this inline — but kept as the
seam for a per-tag size registration should Collector gain a
memory-attribution pass.
*/
size_t IoObject_memorySizeFunc(IoObject *self) {
    return sizeof(IoObjectData) + (IoObject_ownsSlots(self)
                                       ? PHash_memorySize(IoObject_slots(self))
                                       : 0);
}

/*cdoc Object IoObject_compactFunc(self)
Tag compactFunc candidate that rehashes the slots PHash to its minimum
size. Currently not wired up from IoObject_compact (the dispatch is
commented out), but available for a future explicit compact pass.
*/
void IoObject_compactFunc(IoObject *self) {
    PHash_compact(IoObject_slots(self));
}

// proto methods ----------------------------------------------

IO_METHOD(IoObject, protoPerform) {
    /*doc Object perform(methodName, <arg1>, <arg2>, ...)
    Performs the method corresponding to methodName with the arguments supplied.
    */

    IoObject *slotName = IoMessage_locals_valueArgAt_(m, locals, 0);

    if (ISMESSAGE(slotName)) {
        IOASSERT(IoMessage_argCount(m) == 1,
                 "perform takes a single argument when using a Message as an "
                 "argument");
        return IoObject_perform(self, locals, slotName);
    }

    IOASSERT(ISSYMBOL(slotName),
             "perform requires a Symbol or Message argument");

    {
        IoObject *context;
        IoObject *v = IoObject_rawGetSlot_context_(self, slotName, &context);
        IoMessage *newMessage = IoMessage_newWithName_label_(
            IOSTATE, slotName, IoMessage_rawLabel(m));
        IoMessage_rawSetLineNumber_(newMessage, IoMessage_rawLineNumber(m));
        if (v) {
            int i;
            List *args = IoMessage_rawArgList(m);

            for (i = 1; i < List_size(args); i++) {
                IoMessage_addArg_(newMessage,
                                  IoMessage_deepCopyOf_(List_at_(args, i)));
            }

            return IoObject_activate(v, self, locals, newMessage, context);
        }

        return IoObject_forward(self, locals, newMessage);
    }

    return IoObject_forward(self, locals, m);
}

IO_METHOD(IoObject, protoPerformWithArgList) {
    /*doc Object performWithArgList(methodName, argList)
    Performs the method corresponding to methodName with the arguments in the
    argList.
    */

    IoSymbol *slotName = IoMessage_locals_symbolArgAt_(m, locals, 0);
    if (IOSTATE->errorRaised) return IONIL(self);
    IoList *args = IoMessage_locals_listArgAt_(m, locals, 1);
    if (IOSTATE->errorRaised) return IONIL(self);
    List *argList = IoList_rawList(args);
    IoObject *context;
    IoObject *v = IoObject_rawGetSlot_context_(self, slotName, &context);

    if (v) {
        IoMessage *newMessage = IoMessage_newWithName_(IOSTATE, slotName);
        size_t i, max = List_size(argList);

        for (i = 0; i < max; i++) {
            IoMessage_addCachedArg_(newMessage, LIST_AT_(argList, i));
        }

        return IoObject_activate(v, self, locals, newMessage, context);
    }

    return IoObject_forward(self, locals, m);
}

IO_METHOD(IoObject, protoWrite) {
    /*doc Object write(<any number of arguments>)
    Sends a print message to the evaluated result of each argument. Returns Nil.
    */

    int n, max = IoMessage_argCount(m);
    IoState *state = IOSTATE;

    for (n = 0; n < max; n++) {
        IoObject *v = IoMessage_locals_valueArgAt_(m, locals, n);
        IoMessage_locals_performOn_(state->printMessage, locals, v);
    }

    return IONIL(self);
}

IO_METHOD(IoObject, protoWriteLn) {
    /*doc Object writeln(<any number of arguments>)
    Same as write() but also writes a return character at the end. Returns Nil.
    */

    IoObject_protoWrite(self, locals, m);
    IoState_print_(IOSTATE, "\n");
    return IONIL(self);
}

/*cdoc Object IoObject_initClone_(self, locals, m, newObject)
Runs the newly cloned object's `init` slot, if any, with the original
caller's locals/message. Called by IoObject_clone (but not by the
lower-level IOCLONE) so that user types can always rely on init firing
after a surface-level `clone` message.
*/
// inline
IoObject *IoObject_initClone_(IoObject *self, IoObject *locals, IoMessage *m,
                              IoObject *newObject) {
    IoState *state = IOSTATE;
    IoObject *context;
    IoObject *initSlotValue = IoObject_rawGetSlot_context_(
        newObject, IoMessage_name(state->initMessage), &context);

    if (initSlotValue) {
        IoObject_activate(initSlotValue, newObject, locals, state->initMessage,
                          context);
    }

    return newObject;
}

/*cdoc Object IOCLONE(self)
GC-safe clone primitive. Pauses the collector around the tag's clone
function so an incremental mark in the middle of allocation cannot
see the half-built object; afterward addValueIfNecessary ensures the
new object is registered with the collector before the pause is
released. Every primitive's cloneFunc is invoked through this path.
*/
IoObject *IOCLONE(IoObject *self) {
    IoState *state = IOSTATE;
    IoObject *newObject;

    IoState_pushCollectorPause(state);
    newObject = IoObject_tag(self)->cloneFunc(self);
    IoState_addValueIfNecessary_(state, newObject);
    IoState_popCollectorPause(state);
    return newObject;
}

IO_METHOD(IoObject, clone) {
    /*doc Object clone
    Returns a clone of the receiver.
    */

    IoObject *newObject = IOCLONE(self);
    return IoObject_initClone_(self, locals, m, newObject);
}

IO_METHOD(IoObject, cloneWithoutInit) {
    /*doc Object cloneWithoutInit
    Returns a clone of the receiver but does not call init.
    */

    return IOCLONE(self);
}

IO_METHOD(IoObject, shallowCopy) {
    /*doc Object shallowCopy
    Returns a shallow copy of the receiver.
    */

    IOASSERT(ISOBJECT(self), "shallowCopy doesn't work on primitives");

    {
        IoObject *newObject = IoObject_new(IOSTATE);
        PHASH_FOREACH(IoObject_slots(self), k, v,
                      IoObject_setSlot_to_(newObject, k, v));
        return newObject;
    }
}

// lobby methods ----------------------------------------------

IO_METHOD(IoObject, protoSet_to_) {
    /*doc Object setSlot(slotNameString, valueObject)
    Sets the slot slotNameString in the receiver to
    hold valueObject. Returns valueObject.
    */

    IoSymbol *slotName = IoMessage_locals_symbolArgAt_(m, locals, 0);
    if (IOSTATE->errorRaised) return IONIL(self);
    IoObject *slotValue = IoMessage_locals_valueArgAt_(m, locals, 1);
    if (IOSTATE->errorRaised) return IONIL(self);
    IoObject_inlineSetSlot_to_(self, slotName, slotValue);
    IOSTATE->slotVersion++;
    return slotValue;
}

IO_METHOD(IoObject, protoSetSlotWithType) {
    /*doc Object setSlotWithType(slotNameString, valueObject)
    Sets the slot slotNameString in the receiver to
    hold valueObject and sets the type slot of valueObject
    to be slotNameString. Returns valueObject.
    */

    IoSymbol *slotName = IoMessage_locals_symbolArgAt_(m, locals, 0);
    if (IOSTATE->errorRaised) return IONIL(self);
    IoObject *slotValue = IoMessage_locals_valueArgAt_(m, locals, 1);
    if (IOSTATE->errorRaised) return IONIL(self);
    IoObject_inlineSetSlot_to_(self, slotName, slotValue);
    IoObject_createSlotsIfNeeded(slotValue);
    if (PHash_at_(IoObject_slots(slotValue), IOSTATE->typeSymbol) == NULL) {
        IoObject_inlineSetSlot_to_(slotValue, IOSTATE->typeSymbol, slotName);
    }
    IOSTATE->slotVersion++;
    return slotValue;
}

IO_METHOD(IoObject, localsUpdateSlot) {
    /*doc Object localsUpdateSlot(slotNameString, valueObject)
    Local's version of updateSlot mthod.
    */

    IoSymbol *slotName = IoMessage_locals_firstStringArg(m, locals);
    // IoSymbol *slotName  = IoMessage_locals_symbolArgAt_(m, locals, 0);
    IoObject *obj = IoObject_rawGetSlot_(self, slotName);

    if (obj) {
        // IoObject *slotValue = IoMessage_locals_valueArgAt_(m, locals, 1);
        IoObject *slotValue = IoMessage_locals_quickValueArgAt_(m, locals, 1);
        IoObject_inlineSetSlot_to_(self, slotName, slotValue);
        return slotValue;
    } else {
        IoObject *theSelf = IoObject_rawGetSlot_(self, IOSTATE->selfSymbol);

        if (theSelf) {
            return IoObject_perform(theSelf, locals, m);
        }
    }

    IoState_error_(IOSTATE, m,
                   "updateSlot - slot with name `%s' not found in `%s'. Use := "
                   "to create slots.",
                   CSTRING(slotName), IoObject_name(self));

    return IONIL(self);
}

IO_METHOD(IoObject, protoUpdateSlot_to_) {
    /*doc Object updateSlot(slotNameString, valueObject)
    Same as setSlot(), but raises an error if the slot does not
    already exist in the receiver's slot lookup path.
    */

    IoSymbol *slotName = IoMessage_locals_firstStringArg(m, locals);
    IoObject *slotValue = IoMessage_locals_quickValueArgAt_(m, locals, 1);
    IoObject *obj = IoObject_rawGetSlot_(self, slotName);

    if (obj) {
        IoObject_inlineSetSlot_to_(self, slotName, slotValue);
        IOSTATE->slotVersion++;
    } else {
        IoState_error_(IOSTATE, m,
                       "Slot %s not found. Must define slot using := operator "
                       "before updating.",
                       CSTRING(slotName));
    }

    return slotValue;
}

IO_METHOD(IoObject, protoGetSlot_) {
    /*doc Object getSlot(slotNameString)
    Returns the value of the slot named slotNameString
    (following the lookup path) or nil if no such slot is found.
    */

    IoSymbol *slotName = IoMessage_locals_symbolArgAt_(m, locals, 0);
    return IoObject_getSlot_(self, slotName);
}

IO_METHOD(IoObject, protoGetLocalSlot_) {
    /*doc Object getLocalSlot(slotNameString)
    Returns the value of the slot named slotNameString
    (not looking in the object's protos) or nil if no such slot is found.
    */

    IoSymbol *slotName = IoMessage_locals_symbolArgAt_(m, locals, 0);

    if (IoObject_ownsSlots(self)) {
        IoObject *v = PHash_at_(IoObject_slots(self), slotName);
        if (v)
            return v;
    }

    return IONIL(self);
}

IO_METHOD(IoObject, protoHasLocalSlot) {
    /*doc Object hasLocalSlot(slotNameString)
    Returns true if the slot exists in the receiver or false otherwise.
    */

    IoSymbol *slotName = IoMessage_locals_symbolArgAt_(m, locals, 0);
    IoObject_createSlotsIfNeeded(self);
    return IOBOOL(self, PHash_at_(IoObject_slots(self), slotName) != NULL);
}

IO_METHOD(IoObject, protoRemoveSlot) {
    /*doc Object removeSlot(slotNameString)
    Removes the specified slot (only) in the receiver if it exists. Returns
    self.
    */

    IoSymbol *slotName = IoMessage_locals_symbolArgAt_(m, locals, 0);
    IoObject_createSlotsIfNeeded(self);
    PHash_removeKey_(IoObject_slots(self), slotName);
    return self;
}

IO_METHOD(IoObject, protoRemoveAllSlots) {
    /*doc Object removeAllSlots
    Removes all of the receiver's slots. Returns self.
    */

    PHash_clean(IoObject_slots(self));
    return self;
}

IO_METHOD(IoObject, protoSlotNames) {
    /*doc Object slotNames
    Returns a list of strings containing the names of the
    slots in the receiver (but not in its lookup path).
    */

    IoObject_createSlotsIfNeeded(self);

    {
        IoList *slotNames = IoList_new(IOSTATE);
        PHASH_FOREACH(IoObject_slots(self), key, value,
                      IoList_rawAppend_(slotNames, key););
        return slotNames;
    }
}

IO_METHOD(IoObject, protoSlotValues) {
    /*doc Object slotValues
    Returns a list of the values held in the slots of the receiver.
    */

    IoObject_createSlotsIfNeeded(self);

    {
        IoList *slotNames = IoList_new(IOSTATE);
        PHASH_FOREACH(IoObject_slots(self), key, value,
                      IoList_rawAppend_(slotNames, value););
        return slotNames;
    }
}

/*doc Object forward
Called when the receiver is sent a message it doesn't recognize.
Default implementation raises an "Object doesNotRespond" exception.
Subclasses can override this method to implement proxies or special error
handling. <p> Example: <p> <pre> myProxy forward = method( messageName :=
thisMessage name arguments := thisMessage arguments myObject
doMessage(thisMessage)
)
</pre>
*/

/*
IO_METHOD(IoObject, forward_)
{

        IoState_error_(IOSTATE, m, "%s does not respond to message '%s'",
                                   IoObject_name(self),
                                   CSTRING(IoMessage_name(m)));
        return IONIL(self);
}
*/

IO_METHOD(IoObject, ancestorWithSlot) {
    /*doc Object ancestorWithSlot(slotName)
    Returns the first ancestor of the receiver that contains
    a slot of the specified name or Nil if none is found.
    */

    IoObject *slotName = IoMessage_locals_symbolArgAt_(m, locals, 0);
    IoObject **proto = IoObject_protos(self);

    while (*proto) {
        IoObject *context = NULL;
        IoObject *v =
            IoObject_rawGetSlot_context_((*proto), slotName, &context);

        if (v) {
            return context;
        }

        proto++;
    }

    return IONIL(self);
}

IO_METHOD(IoObject, contextWithSlot) {
    /*doc Object contextWithSlot(slotName)
    Returns the first context (starting with the receiver and following the
    lookup path) that contains a slot of the specified name or Nil if none is
    found.
    */

    IoObject *slotName = IoMessage_locals_symbolArgAt_(m, locals, 0);
    IoObject *context = NULL;
    IoObject_rawGetSlot_context_(self, slotName, &context);
    return context ? context : IONIL(self);
}

// ---------------------------------------------------------------------------

/*cdoc Object IoObject_rawDoString_label_(self, string, label)
Compiles a source string into a message tree (via Compiler
messageForString) and evaluates it against self. Used by the bootstrap
/ recursive-fallback paths of doString and doFile; the iterative path
instead parses directly with IoMessage_newFromText_labelSymbol_ and
hands off to the eval loop via FRAME_STATE_DO_EVAL. label becomes the
Message's source label for error reporting.
*/
IoObject *IoObject_rawDoString_label_(IoObject *self, IoSymbol *string,
                                      IoSymbol *label) {
    IoMessage *cm = NULL;
    IoMessage *messageForString = NULL;
    IoMessage *newMessage = NULL;
    IoState *state = IOSTATE;

    if (!ISSEQ(string)) {
        IoState_error_(
            state, NULL,
            "IoObject_rawDoString_label_ requires a string argument");
    }

    {
        IoSymbol *internal;
        IoState_pushCollectorPause(state);

        internal = IOSYMBOL("[internal]");
        cm =
            IoMessage_newWithName_label_(state, IOSYMBOL("Compiler"), internal);
        messageForString = IoMessage_newWithName_label_(
            state, IOSYMBOL("messageForString"), internal);

        IoMessage_rawSetNext_(cm, messageForString);
        IoMessage_addCachedArg_(messageForString, string);
        IoMessage_addCachedArg_(messageForString, label);

        newMessage = IoMessage_locals_performOn_(cm, self, self);
        IoState_stackRetain_(state, newMessage); // needed?

        IoState_popCollectorPause(state);

        if (newMessage) {
            return IoMessage_locals_performOn_(newMessage, self, self);
        }

        IoState_error_(state, NULL, "no message compiled\n");
        return IONIL(self);
    }
}

IO_METHOD(IoObject, doMessage) {
    /*doc Object doMessage(aMessage, optionalContext)
    Evaluates the message object in the context of the receiver.
    Returns the result. optionalContext can be used to specific the locals
    context in which the message is evaluated.
    */

    IoState *state = IOSTATE;

    // Get the message argument (unevaluated - it's a message literal)
    IoMessage *aMessage = IoMessage_locals_messageArgAt_(m, locals, 0);
    IoObject *context = self;

    // Get optional context argument (may need evaluation)
    if (IoMessage_argCount(m) >= 2) {
        context = IoMessage_locals_valueArgAt_(m, locals, 1);
    }

    // Use iterative path if eval loop is active, otherwise recursive fallback
    if (state->currentFrame != NULL) {
        if (FRAME_DATA(state->currentFrame)->message == m) {
            // Called directly from eval loop - use frame-state (zero C stack growth)
            IoEvalFrame *frame = state->currentFrame;
            IoEvalFrameData *fd = FRAME_DATA(frame);
            fd->controlFlow.doInfo.codeMessage = aMessage;
            fd->controlFlow.doInfo.evalTarget = self;
            fd->controlFlow.doInfo.evalLocals = context;
            fd->state = FRAME_STATE_DO_EVAL;
            state->needsControlFlowHandling = 1;
            return state->ioNil;
        }
        // Called indirectly (e.g. from interpolate) - use nested eval
        return IoMessage_locals_performOn_iterative(aMessage, context, self);
    }

    return IoMessage_locals_performOn_(aMessage, context, self);
}

IO_METHOD(IoObject, doString) {
    /*doc Object doString(aString)
    Evaluates the string in the context of the receiver. Returns the result.
    */

    IoState *state = IOSTATE;

    // Get the string argument (may need evaluation if not a literal)
    IoSymbol *string = IoMessage_locals_seqArgAt_(m, locals, 0);
    IoSymbol *label;

    if (IoMessage_argCount(m) > 1) {
        label = IoMessage_locals_symbolArgAt_(m, locals, 1);
    } else {
        label = IOSYMBOL("doString");
    }

    // Use iterative path if eval loop is active, otherwise recursive fallback
    if (state->currentFrame != NULL) {
        IoState_pushCollectorPause(state);
        IoMessage *codeMsg = IoMessage_newFromText_labelSymbol_(state,
            CSTRING(string), label);
        IoState_popCollectorPause(state);

        if (!codeMsg) {
            IoState_error_(state, m, "doString: failed to compile string");
            return state->ioNil;
        }

        if (FRAME_DATA(state->currentFrame)->message == m) {
            // Called directly from eval loop - use frame-state (zero C stack growth)
            IoEvalFrame *frame = state->currentFrame;
            IoEvalFrameData *fd = FRAME_DATA(frame);
            fd->controlFlow.doInfo.codeMessage = codeMsg;
            fd->controlFlow.doInfo.evalTarget = self;
            fd->controlFlow.doInfo.evalLocals = self;
            fd->state = FRAME_STATE_DO_EVAL;
            state->needsControlFlowHandling = 1;
            return state->ioNil;
        }
        // Called indirectly (e.g. from interpolate) - use nested eval
        return IoMessage_locals_performOn_iterative(codeMsg, self, self);
    }

    IoObject *result;
    IoState_pushRetainPool(state);
    result = IoObject_rawDoString_label_(self, string, label);
    IoState_popRetainPoolExceptFor_(state, result);
    return result;
}

IO_METHOD(IoObject, doFile) {
    /*doc Object doFile(pathString)
    Evaluates the File in the context of the receiver. Returns the result.
    pathString is relative to the current working directory.
    */

    IoState *state = IOSTATE;

    IoSymbol *path = IoMessage_locals_symbolArgAt_(m, locals, 0);
    if (state->errorRaised) return state->ioNil;

    IoFile *file = IoFile_newWithPath_(state, path);
    IoObject *contents = IoFile_contents(file, locals, m);
    if (state->errorRaised) return state->ioNil;

    IoSymbol *string = (IoSymbol *)IoSeq_rawAsSymbol(contents);

    if (!IoSeq_rawSize(string)) {
        return IONIL(self);
    }

    // Use iterative path if eval loop is active, otherwise recursive fallback
    if (state->currentFrame != NULL) {
        IoState_pushCollectorPause(state);
        IoMessage *codeMsg = IoMessage_newFromText_labelSymbol_(state,
            CSTRING(string), path);
        IoState_popCollectorPause(state);

        if (!codeMsg) {
            IoState_error_(state, m, "doFile: failed to compile file %s",
                          CSTRING(path));
            return state->ioNil;
        }

        if (FRAME_DATA(state->currentFrame)->message == m) {
            // Called directly from eval loop - use frame-state (zero C stack growth)
            IoEvalFrame *frame = state->currentFrame;
            IoEvalFrameData *fd = FRAME_DATA(frame);
            fd->controlFlow.doInfo.codeMessage = codeMsg;
            fd->controlFlow.doInfo.evalTarget = self;
            fd->controlFlow.doInfo.evalLocals = self;
            fd->state = FRAME_STATE_DO_EVAL;
            state->needsControlFlowHandling = 1;
            return state->ioNil;
        }
        // Called indirectly - use nested eval
        return IoMessage_locals_performOn_iterative(codeMsg, self, self);
    }

    return IoObject_rawDoString_label_(self, string, path);
}

IO_METHOD(IoObject, isIdenticalTo) {
    /*doc Object isIdenticalTo(aValue)
    Returns true if the receiver is identical to aValue, false otherwise.
    */

    IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);
    return IOBOOL(self, self == other);
}

IO_METHOD(IoObject, equals) {
    /*doc Object ==(aValue)
    Returns true if receiver and aValue are equal, false otherwise.
*/

    IOASSERT(IoMessage_argCount(m), "compare requires argument");

    {
        IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);
        return IOBOOL(self, IoObject_compare(self, other) == 0);
    }
}

IO_METHOD(IoObject, notEquals) {
    /*doc Object !=(aValue)
    Returns true the receiver is not equal to aValue, false otherwise.
    */

    IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);
    return IOBOOL(self, IoObject_compare(self, other) != 0);
}

IO_METHOD(IoObject, foreachSlot) {
    /*doc Object foreach([name,] value, message)
    For each slot, set name to the slot's
    name and value to the slot's value and execute message. Examples:
    <p>
    <pre>
    myObject foreach(n, v,
            writeln("slot ", n, " = ", v type)
    )

    myObject foreach(v,
            writeln("slot type ", v type)
    )
    </pre>
    */

    IoSymbol *keyName;
    IoSymbol *valueName;
    IoMessage *doMessage;
    IoObject *result = IONIL(self);

    IoState_pushRetainPool(IOSTATE);
    IoMessage_foreachArgs(m, self, &keyName, &valueName, &doMessage);
    if (IOSTATE->errorRaised) {
        IoState_popRetainPool(IOSTATE);
        return IONIL(self);
    }

    PHASH_FOREACH(
        IoObject_slots(self), key, value, IoState_clearTopPool(IOSTATE);

        if (keyName) { IoObject_setSlot_to_(locals, keyName, key); }

        IoObject_setSlot_to_(locals, valueName, value);
        result = IoMessage_locals_performOn_(doMessage, locals, locals);

        if (IoState_handleStatus(IOSTATE)) { goto done; });
done:
    IoState_popRetainPoolExceptFor_(IOSTATE, result);
    return result;
}

IO_METHOD(IoObject, subtract) {
    /*doc Object -(aNumber)
    Returns the negative version of aNumber.
    Raises an exception if argument is not a number.
    */

    IoNumber *num = IoMessage_locals_numberArgAt_(m, locals, 0);
    return IONUMBER(-IoNumber_asDouble(num));
}

IO_METHOD(IoObject, self) {
    /*doc Object self
    Returns self.
    */
    /*doc Object thisContext
    Synonym to self.
    */

    return self;
}

IO_METHOD(IoObject, thisMessage) {
    /*doc Object thisMessage
    Returns the calling message (i.e. thisMessage itself, huh).
    */

    return m;
}

IO_METHOD(IoObject, locals) {
    /*doc Object thisLocalContext
    Returns current locals.
    */

    return locals;
}

// message callbacks --------------------------------------

/*cdoc Object IoObject_name(self)
Returns the object's display name, preferring a `type` slot containing
a Sequence over the tag's underlying name. This lets Io code rename
"Object" subclasses (e.g. `Point := Object clone do(type := "Point")`)
and have error messages and type reflection show the user-visible name.
*/
const char *IoObject_name(IoObject *self) {
    // If self has a type slot which is a string, then use that instead of the
    // tag name
    IoObject *type = IoObject_rawGetSlot_(self, IOSYMBOL("type"));
    if (type && ISSEQ(type)) {
        return CSTRING(type);
    }

    return IoTag_name(IoObject_tag(self));
}

/*cdoc Object IoObject_compare(self, v)
Total-order comparison used by < > == etc. Short-circuits on pointer
identity, dispatches to the tag's compareFunc when one is registered
(Number, Seq, List, etc. provide type-aware orderings), and falls
back to IoObject_defaultCompare which orders by tag then by pointer.
*/
int IoObject_compare(IoObject *self, IoObject *v) {
    if (self == v) {
        return 0;
    }

    if (IoObject_tag(self)->compareFunc) {
        return (IoObject_tag(self)->compareFunc)(self, v);
    }

    return IoObject_defaultCompare(self, v);
}

/*cdoc Object IoObject_defaultCompare(self, v)
Fallback ordering for Objects without a tag compareFunc. Compares
tag pointers first (so same-type objects cluster) and breaks ties by
raw pointer, returning -1/0/1. Not semantically meaningful, just
stable enough to let sort and Map rely on a total order.
*/
int IoObject_defaultCompare(IoObject *self, IoObject *v) {

    // IoState_error_(IOSTATE, NULL, "attempt to compare %s to %s",
    // IoObject_name(self), IoObject_name(v)); return 0;

    ptrdiff_t d = -((ptrdiff_t)IoObject_tag(self) - (ptrdiff_t)IoObject_tag(v));
    // printf("warning: IoObject_defaultCompare attempt to compare %s to %s\n",
    // IoObject_name(self), IoObject_name(v));

    if (d == 0) {
        d = ((ptrdiff_t)self) - ((ptrdiff_t)v);
    }

    if (d == 0)
        return 0;

    return d > 0 ? 1 : -1;
}

/*cdoc Object IoObject_sortCompare(self, v)
Pointer-to-pointer adapter matching qsort's expected signature. Lets
IoList and similar sort by IoObject_compare without per-call wrappers.
*/
int IoObject_sortCompare(IoObject **self, IoObject **v) {
    return IoObject_compare(*self, *v);
}

/*cdoc Object IoObject_compact(self)
Placeholder for a per-tag compact pass. The tag dispatch is currently
commented out; the function stays as a seam so callers need not be
changed if compaction is re-enabled.
*/
void IoObject_compact(IoObject *self) {
    /*
    if (IoObject_tag(self)->compactFunc)
    {
            (IoObject_tag(self)->compactFunc)(self);
    }
    */
}

// lobby methods ----------------------------------------------

IO_METHOD(IoObject, memorySizeMethod) {
    /*doc Object memorySize
    Return the amount of memory used by the object.
    */

    return IONUMBER(IoObject_memorySize(self));
}

/*doc Object checkMemory()
        Accesses memory in the IoObjectData struct that should be accessible.
   Should cause a memory access exception if memory is corrupt.
        */
/*cdoc Object IoObject_rawCheckMemory(self)
Touches IoObject state so a memory access exception fires if the
object's memory has been freed or corrupted. Invoked on every live
marker by IoCollector checkMemory.
*/
int IoObject_rawCheckMemory(IoObject *self) { return IOCOLLECTOR != 0x0; }

IO_METHOD(IoObject, compactMethod) {
    /*doc Object compact
    Compact the memory for the object if possible. Returns self.
    */

    IoObject_compact(self);
    return self;
}

IO_METHOD(IoObject, type) {
    /*doc Object type
    Returns a string containing the name of the type of Object (Number, String,
    etc).
    */

    return IOSYMBOL((char *)IoObject_name(self));
}

/*cdoc Object IoObject_defaultPrint(self)
Built-in print implementation used when no Io-level `print` slot is
defined. Formats Symbols and Numbers via their primitive printers and
every other object as "name_pointer", with Messages also showing their
name. This is the function the C side calls when `print` is not
overridden in an object's proto chain.
*/
void IoObject_defaultPrint(IoObject *self) {
    if (ISSYMBOL(self)) {
        IoSeq_rawPrint(self);
    } else if (ISNUMBER(self)) {
        IoNumber_print(self);
    } else {
        IoState_print_(IOSTATE, "%s_%p", IoObject_name(self), self);

        if (ISMESSAGE(self)) {
            IoState_print_(IOSTATE, " '%s'", CSTRING(IoMessage_name(self)));
        }
    }
}

/*cdoc Object IoObject_print(self)
Invokes the receiver's `print` slot via the cached printMessage. Uses
self as its own locals — a deliberate hack because this is called from
debug / REPL paths that may not have a real locals at hand.
*/
void IoObject_print(IoObject *self) {
    IoMessage_locals_performOn_(IOSTATE->printMessage, self, self);
    // using self as locals hack
}

/*
IO_METHOD(IoObject, truthValueOfArg);
{
        //-doc Object The '' method evaluates the argument and returns the
result.

        IOASSERT(IoMessage_argCount(m) > 0, "argument required");
        return IoMessage_locals_valueArgAt_(m, locals, 0);
}
*/

IO_METHOD(IoObject, evalArg) {
    /*doc Object (expression)
    The '' method evaluates the argument and returns the result.
    */

    /*doc Object evalArg(expression)
    The '' method evaluates the argument and returns the result.
    */

    IOASSERT(IoMessage_argCount(m) > 0, "argument required");
    /* eval the arg and return a non-Nil so an attached else() won't get
     * performed */
    return IoMessage_locals_valueArgAt_(m, locals, 0);
}

IO_METHOD(IoObject, evalArgAndReturnSelf) {
    /*doc Object evalArgAndReturnSelf(expression)
    Evaluates the argument and returns the target.
    */

    IoObject_evalArg(self, locals, m);
    return self;
}

IO_METHOD(IoObject, evalArgAndReturnNil) {
    /*doc Object evalArgAndReturnNil(expression)
    Evaluates the argument and returns nil.
    */

    IoObject_evalArg(self, locals, m);
    return IONIL(self);
}

IO_METHOD(IoObject, isLessThan_) {
    /*doc Object <(expression)
    Evaluates argument and returns self if self is less or Nil if not.
    */

    IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 0);
    return IOBOOL(self, IoObject_compare(self, v) < 0);
}

IO_METHOD(IoObject, isLessThanOrEqualTo_) {
    /*doc Object <=(expression)
    Evaluates argument and returns self if self is less
    than or equal to it, or Nil if not.
    */

    IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 0);
    return IOBOOL(self, IoObject_compare(self, v) <= 0);
}

IO_METHOD(IoObject, asBoolean) {
    if (self == IOSTATE->ioFalse || self == IOSTATE->ioNil)
        return self;
    else
        return IOSTATE->ioTrue;
}

IO_METHOD(IoObject, isGreaterThan_) {
    /*doc Object >(expression)
    Evaluates argument and returns self if self is greater than it, or Nil if
    not.
    */

    IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 0);
    return IOBOOL(self, IoObject_compare(self, v) > 0);
}

IO_METHOD(IoObject, isGreaterThanOrEqualTo_) {
    /*doc Object >=(expression)
    Evaluates argument and returns self if self is greater
    than or equal to it, or Nil if not.
    */

    IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 0);
    return IOBOOL(self, IoObject_compare(self, v) >= 0);
}

IO_METHOD(IoObject, uniqueId) {
    /*doc Object uniqueId
    Returns a Number containing a unique id for the receiver.
    */
    char s[32];
    sprintf(s, "%p", (void *)IoObject_deref(self));
    return IOSYMBOL(s);
    // return IONUMBER((double)((size_t)IoObject_deref(self)));
}

IO_METHOD(IoObject, do) {
    /*doc Object do(expression)
    Evaluates the message in the context of the receiver. Returns self.
    */

    if (IoMessage_argCount(m) != 0) {
        IoMessage *argMessage = IoMessage_rawArgAt_(m, 0);
        IoMessage_locals_performOn_(argMessage, self, self);
    }

    return self;
}

IO_METHOD(IoObject, lexicalDo) {
    /*doc Object lexicalDo(expression)
    Evaluates the message in the context of the receiver.
    The lexical context is added as a proto of the receiver while the argument
    is evaluated. Returns self.
    */

    if (IoMessage_argCount(m) != 0) {
        IoMessage *argMessage = IoMessage_rawArgAt_(m, 0);
        IoObject_rawAppendProto_(self, locals);
        IoMessage_locals_performOn_(argMessage, self, self);
        IoObject_rawRemoveProto_(self, locals);
    }

    return self;
}

IO_METHOD(IoObject, message) {
    /*doc Object message(expression)
    Return the message object for the argument or Nil if there is no argument.
    Note: returned object is a mutable singleton. Use "message(foo) clone" if
you wish to modify it.
    */

    return IoMessage_argCount(m) ? IoMessage_rawArgAt_(m, 0) : IONIL(self);
}

// inline these -------------------------------------------------

/*cdoc Object IoObject_hasCloneFunc_(self, func)
Class test by tag cloneFunc identity. Useful when the ISXXX macros
aren't available (e.g. across translation units that don't include
the primitive's header). Exactly how argIsCall checks for Call objects.
*/
int IoObject_hasCloneFunc_(IoObject *self, IoTagCloneFunc *func) {
    return (IoObject_tag(self)->cloneFunc == func);
}

// --------------------------------------------

/*cdoc Object IoObject_markColorName(self)
Returns the collector's color for self (white/gray/black) as a string.
Debug aid for watching tri-color transitions during a sweep.
*/
char *IoObject_markColorName(IoObject *self) {
    return Collector_colorNameFor_(IOCOLLECTOR, self);
}

void IoSymbol_println(IoSymbol *self) { printf("%s\n", CSTRING(self)); }

/*cdoc Object IoObject_show(self)
Prints the object's address, name, and the names of all slot keys.
Exercised from debug tracing paths when an object's identity matters
more than its full contents.
*/
void IoObject_show(IoObject *self) {
    printf("  %p %s\n", (void *)self, IoObject_name(self));
    // PHash_doOnKeys_(IoObject_slots(self), (PHashDoCallback
    // *)IoSymbol_println);
    PHASH_FOREACH(IoObject_slots(self), k, v, IoSymbol_println(k));
}

IO_METHOD(IoObject, setIsActivatableMethod) {
    /*doc Object setIsActivatable(aValue)
    When called with a non-Nil aValue, sets the object
    to call its activate slot when accessed as a value. Turns this behavior
    off if aValue is Nil. Only works on Objects which are not Activatable
    Primitives (such as CFunction or Block). Returns self.
    */

    IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 0);
    IoObject *objectProto = IoState_protoWithId_(IOSTATE, protoId);

    IoTag_activateFunc_(IoObject_tag(objectProto),
                        (IoTagActivateFunc *)IoObject_activateFunc);

    IoObject_isActivatable_(self, ISTRUE(v)) return self;
}

IO_METHOD(IoObject, isActivatableMethod) {
    /*doc Object isActivatable
    Returns true if the receiver is activatable, false otherwise.
    */

    return IoObject_isActivatable(self) ? IOTRUE(self) : IOFALSE(self);
}

/*
IoNumber *IoObject_getNumberSlot(IoObject *self,
                                                                IoObject
*locals, IoMessage *m, IoSymbol *slotName)
{
        IoObject *v  = IoObject_getSlot_(self, slotName);
        IOASSERT(ISNUMBER(v),  CSTRING(slotName));
        return v;
}
*/

/*cdoc Object IoObject_rawGetUArraySlot(self, locals, m, slotName)
Read a Sequence-valued slot and return its backing UArray. IOASSERT
raises an Io-level exception if the slot is missing or not a Sequence.
Used by primitives that want to work on the raw byte buffer without
going through the Seq wrapper.
*/
UArray *IoObject_rawGetUArraySlot(IoObject *self, IoObject *locals,
                                  IoMessage *m, IoSymbol *slotName) {
    IoSeq *seq = IoObject_getSlot_(self, slotName);
    IOASSERT(ISSEQ(seq), CSTRING(slotName));
    return IoSeq_rawUArray(seq);
}

/*cdoc Object IoObject_rawGetMutableUArraySlot(self, locals, m, slotName)
Mutable-access variant of IoObject_rawGetUArraySlot. Identical body
today; the separate name documents intent at the call site and leaves
room for a future copy-on-write check.
*/
UArray *IoObject_rawGetMutableUArraySlot(IoObject *self, IoObject *locals,
                                         IoMessage *m, IoSymbol *slotName) {
    IoSeq *seq = IoObject_getSlot_(self, slotName);
    IOASSERT(ISSEQ(seq), CSTRING(slotName));
    return IoSeq_rawUArray(seq);
}

IO_METHOD(IoObject, argIsActivationRecord) {
    /*doc Object argIsActivationRecord
    Note: seems to be an obsolete method.
          */
    return IOBOOL(self,
                  PHash_at_(IoObject_slots(self), IOSTATE->callSymbol) != NULL);
}

/*void *testStack(void *a)
{
        int v[256];
        memset(v, 0x1, 128);
        return a + v[0];
}*/

IO_METHOD(IoObject, argIsCall) {
    /*doc Object argIsCall(arg)
          Returns true if arg is an activation context (i.e. Call object)
          <br/>
          Note: this is used internally in one place only (Coroutine callStack).
          Refactoring should be considered.
          */

    IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 0);
    // printf("IoObject_tag(v)->name = '%s'\n", IoObject_tag(v)->name);

    /*
            printf("Collector_checkObjectPointers\n");
            Collector_check(IOSTATE->collector);
            Collector_checkObjectPointers(IOSTATE->collector);
            printf("Collector_checkObjectPointers done\n");
            printf("self = %p\n", self);
            printf("v    = %p\n", v);
            printf("tag  = %p\n", IoObject_tag(self));
            */
    /*
            printf("tag->cloneFunc   = %p\n", IoObject_tag(v)->cloneFunc);
            printf("IoCall_rawClone  = %p\n", IoCall_rawClone);

            //testStack(self);
    <<<<<<< HEAD


            printf("ISACTIVATIONCONTEXT = %i\n", isAct);
    */

    int isAct =
        ((void *)(IoObject_tag(v)->cloneFunc) == (void *)IoCall_rawClone);
    IoObject *t = IOSTATE->ioTrue;  // IOTRUE(self);
    IoObject *f = IOSTATE->ioFalse; // IOFALSE(self);
    // return isAct ? t : f;
    if (isAct) {
        return t;
    }
    return f;
    // return IOBOOL(self, ISACTIVATIONCONTEXT(v));

    /*
            int isAct = ((void *)(IoObject_tag(self)->cloneFunc) == (void
    *)IoCall_rawClone);

    //	printf("ISACTIVATIONCONTEXT = %i\n", isAct);

            void *t = IOTRUE(self);
            void *f = IOFALSE(self);
            return isAct ? t : f;
    */

    return IOBOOL(self, ISACTIVATIONCONTEXT(v));
}

IO_METHOD(IoObject, become) {
    /*doc Object become(anotherObject)
          Replaces receiver with <tt>anotherObject</tt> and returns self.
          Useful for implementing transparent proxies. See also
       <tt>FutureProxy</tt> and <tt>Object @</tt>. <br/> Note: primitives cannot
       become new values.
          */

    IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 0);

    if (self == v || IoObject_deref(v) == IoObject_deref(self))
        return self;

    // IOASSERT(!IoObject_isSymbol(self), "Symbols cannot become new values");
    IOASSERT(ISOBJECT(self), "Primitives cannot become new values");
    // printf("IoObject_become(%p, %p) data %p\n", (void *)self, (void *)v,
    // (void *)IoObject_deref(v));
    IoObject_incrementMarkerCount(v);
    IoObject_dealloc(self);
    CollectorMarker_setObject_(self, IoObject_deref(v));
    return self;
}

/*cdoc Object IoObject_hasDirtySlot_(self, locals, m)
Io-visible test for per-slot dirty tracking used by the persistence
path (IOOBJECT_PERSISTENCE builds). Returns whether the named slot
has been modified since the last markClean.
*/
IOVM_API IoObject *IoObject_hasDirtySlot_(IoObject *self, IoObject *locals,
                                          IoMessage *m) {
    // IoSymbol *slotName = IoMessage_locals_symbolArgAt_(m, locals, 0);
    int result =
        PHash_hasDirtyKey_(IoObject_slots(self),
                           IOREF(IoMessage_locals_symbolArgAt_(m, locals, 0)));
    return IOBOOL(self, result);
}

/*cdoc Object IoObject_protoClean(self)
Clears the object's dirty flag and the per-slot dirty bits. Invoked
by the collector's cleanAllObjects sweep so a persistence layer can
treat the next round of writes as fresh changes.
*/
void IoObject_protoClean(IoObject *self) {
    IoObject_isDirty_(self, 0);
    PHash_cleanSlots(IoObject_slots(self));
}

IO_METHOD(IoObject, markClean) {
    // doc Object markClean Cleans object's slots.
    PHash_cleanSlots(IoObject_slots(self));
    return self;
}

// io_free listeners ---------------------------------------------

/*cdoc Object IoObject_addListener_(self, listener)
Registers an object that wants to be notified (via its tag's
notificationFunc) when self is about to be deallocated. Lazily
allocates the listeners list. Used by weak-reference-style features —
the listener receives a callback in IoObject_dealloc.
*/
void IoObject_addListener_(IoObject *self, void *listener) {
    if (IoObject_listeners(self) == NULL) {
        IoObject_listeners_(self, List_new());
    }

    List_append_(IoObject_listeners(self), listener);
}

/*cdoc Object IoObject_removeListener_(self, listener)
Unregisters a listener and frees the listeners list once it empties,
returning self to its default no-listeners state.
*/
void IoObject_removeListener_(IoObject *self, void *listener) {
    List *listeners = IoObject_listeners(self);

    if (listeners) {
        List_remove_(listeners, listener);

        if (List_size(listeners) == 0) {
            List_free(listeners);
            IoObject_listeners_(self, NULL);
        }
    }
}

// persistence ------------------------------------------------

/*
PID_TYPE IoObject_pid(IoObject *self)
{
        return 0;
}
*/

// asString helper

/*cdoc Object IoObject_asString_(self, m)
C-callable helper that invokes self's `asString` slot via the cached
asStringMessage and validates that the result is a Sequence, raising
an error on m's source location if not. Lets primitive code get a
user-overridable string rendering without duplicating the perform
dance at every call site.
*/
IoSeq *IoObject_asString_(IoObject *self, IoMessage *m) {
    IoSeq *result =
        IoMessage_locals_performOn_(IOSTATE->asStringMessage, self, self);
    /*
    IoSymbol *string = IOSYMBOL("asString");
    IoSymbol *label = string;
    IoSeq *result;

    IoState_pushRetainPool(IOSTATE);

    result = IoObject_rawDoString_label_(self, string, label);
    IoState_popRetainPoolExceptFor_(IOSTATE, result);
    */

    if (!ISSEQ(result)) {
        IoState_error_(IOSTATE, m, "%s asString didn't return a Sequence",
                       IoObject_name(self));
    }

    return result;
}
