
//--metadoc State copyright Steve Dekorte 2002
//--metadoc State license BSD revised

/*cmetadoc State description
Symbol interning and cached-number allocation for the VM. Symbols live
in state->symbols (a CHash keyed by UArray contents) so that equal
Sequences always intern to the same IoObject pointer — essential for
the hot-path message-name comparisons in IoMessage and the iterative
evaluator. Small integer Numbers in [-10, 1024] are pre-created and
shared; larger Numbers go through a bespoke inline allocator that
bypasses IOCLONE (and therefore avoids the collector-pause / tag
dispatch / double-add-to-whites overhead in what is by far the most
common allocation site in typical Io programs).
*/

#include "IoState.h"
#include "IoObject.h"
#include "IoSeq.h"
#include "IoNumber.h"

#define MIN_CACHED_NUMBER -10
#define MAX_CACHED_NUMBER 1024

// numbers ----------------------------------

/*cdoc State IoState_setupCachedNumbers(self)
Pre-creates IoNumber objects for every integer in
[MIN_CACHED_NUMBER, MAX_CACHED_NUMBER] and retains them, so the common
case of integer literals never allocates. Also caches the Number proto
and tag pointers and initializes the freelist used by
IoState_numberWithDouble_ to recycle IoNumberData blocks.
*/
void IoState_setupCachedNumbers(IoState *self) {
    int i;

    self->cachedNumbers = List_new();

    for (i = MIN_CACHED_NUMBER; i < MAX_CACHED_NUMBER + 1; i++) {
        IoNumber *number = IoNumber_newWithDouble_(self, i);
        List_append_(self->cachedNumbers, number);
        IoState_retain_(self, number);
    }

    // Cache Number proto and tag for fast inline allocation
    self->numberProto = IoState_protoWithId_(self, "Number");
    self->numberTag = IoObject_tag(self->numberProto);
    self->numberDataFreeList = NULL;
    self->numberDataFreeListSize = 0;
}

/*cdoc State IoState_numberWithDouble_(self, n)
Fast-path IoNumber allocator. Integer values that fit the cache table
come back as shared singletons (fast pointer compare, zero allocation).
Everything else is built by a manual, pause-free construction that
recycles IoObjectData blocks from state->numberDataFreeList — IOCLONE
on Number was identified as a hotspot in profiling because of the
collector pause, tag activate dispatch, and duplicate add-to-whites
bookkeeping, so this function opens that up by hand. Returned objects
are stack-retained for GC safety.
*/
IoObject *IoState_numberWithDouble_(IoState *self, double n) {
    long i = (long)n;

    if (self->cachedNumbers && i == n && i >= MIN_CACHED_NUMBER &&
        i <= MAX_CACHED_NUMBER) {
        return List_at_(self->cachedNumbers, i - MIN_CACHED_NUMBER);
    }

    // Inline Number allocation: bypass IOCLONE overhead
    // (avoids pushCollectorPause/popCollectorPause which can trigger GC,
    //  tag function dispatch, double Collector_addValue_, redundant field setup)

    // 1. Get a CollectorMarker (recycled from freed list or fresh)
    IoObject *child = Collector_newMarker(self->collector);

    // 2. Get data+protos block from freelist or allocate new
    IoObjectData *data;
    if (self->numberDataFreeList) {
        data = (IoObjectData *)self->numberDataFreeList;
        self->numberDataFreeList = data->data.ptr;
        self->numberDataFreeListSize--;
        // Zero for correctness (flags, listeners, etc. must be 0)
        memset(data, 0, sizeof(IoObjectData) + 2 * sizeof(IoObject *));
    } else {
        data = (IoObjectData *)io_calloc(
            1, sizeof(IoObjectData) + 2 * sizeof(IoObject *));
    }

    // 3. Set up the Number object
    CollectorMarker_setObject_(child, data);
    data->tag = self->numberTag;
    data->data.d = n;
    data->slots = IoObject_slots(self->numberProto);
    // Protos array is inline (immediately after IoObjectData)
    IoObject **protos = (IoObject **)(data + 1);
    protos[0] = self->numberProto;
    data->protos = protos;

    // 4. Stack retain for GC safety (Collector_newMarker already added to whites)
    IoState_unreferencedStackRetain_(self, child);

    return child;
}

// strings ----------------------------------

/*cdoc State IoState_symbolWithUArray_copy_(self, ba, copy)
Canonical symbol-interning entry point. Looks the UArray up in the
symbol table, returns the existing IoSymbol if found (the new ba is
freed unless copy == 1), or creates and registers a new Symbol
otherwise. Does not convert ba to a fixed-width encoding — callers
producing wide/UTF-16 buffers must use the _convertToFixedWidth form.
Result is stack-retained by the caller path.
*/
IoSymbol *IoState_symbolWithUArray_copy_(
    IoState *self, UArray *ba,
    int copy) // carefull - doesn't convert to fixed width
{
    IoSymbol *ioSymbol = CHash_at_(self->symbols, ba);

    if (!ioSymbol) {
        ioSymbol = IoSeq_newSymbolWithUArray_copy_(self, ba, copy);
        return IoState_addSymbol_(self, ioSymbol);
    }

    if (!copy) {
        UArray_free(ba);
    }

    IoState_stackRetain_(self, ioSymbol);
    return ioSymbol;
}

/*cdoc State IoState_symbolWithUArray_copy_convertToFixedWidth(self, ba, copy)
Variant of the UArray-to-Symbol path that first collapses the buffer to
a fixed-width byte encoding before hashing, so two Sequences that render
the same bytes but use different UArray element types still intern to
one Symbol. Forwards to IoState_symbolWithCString_length_.
*/
IoSymbol *IoState_symbolWithUArray_copy_convertToFixedWidth(IoState *self,
                                                            UArray *ba,
                                                            int copy) {
    IoSymbol *r = IoState_symbolWithCString_length_(
        self, (const char *)UArray_bytes(ba), UArray_sizeInBytes(ba));
    if (!copy)
        UArray_free(ba);
    return r;
}

/*cdoc State IoState_symbolWithCString_length_(self, s, length)
Interns a byte slice as a Symbol. Wraps the C buffer in a UTF-8-tagged
UArray, converts it to a fixed-size element type so hashes match the
canonical encoding, then interns through IoState_symbolWithUArray_copy_.
*/
IoSymbol *IoState_symbolWithCString_length_(IoState *self, const char *s,
                                            size_t length) {
    UArray *a =
        UArray_newWithData_type_size_copy_((char *)s, CTYPE_uint8_t, length, 1);
    UArray_setEncoding_(a, CENCODING_UTF8);
    UArray_convertToFixedSizeType(a);
    return IoState_symbolWithUArray_copy_(self, a, 0);
}

/*cdoc State IoState_symbolWithCString_(self, s)
Null-terminated convenience wrapper around
IoState_symbolWithCString_length_. This is the most-used intern path in
the VM — SIOSYMBOL expands to a call here.
*/
IoSymbol *IoState_symbolWithCString_(IoState *self, const char *s) {
    return IoState_symbolWithCString_length_(self, s, strlen(s));
}

/*cdoc State IoState_addSymbol_(self, s)
Registers a freshly-built IoSymbol in the global symbol table and
assigns it two independent hash seeds from state->randomGen. Hash
values are baked into the IoSymbol so PHash cuckoo-hash slot lookups
never re-hash a key — this is the critical invariant that makes slot
reads in the iterative evaluator a pointer-compare plus array lookup.
*/
IoSymbol *IoState_addSymbol_(IoState *self, IoSymbol *s) {
    CHash_at_put_(self->symbols, IoSeq_rawUArray(s), s);
    IoObject_isSymbol_(s, 1);
    s->hash1 = RandomGen_randomInt(self->randomGen) | 0x1;
    s->hash2 = RandomGen_randomInt(self->randomGen) << 1;
    // s->hash2 = rand() << 1; //RandomGen_randomInt(self->randomGen) << 1;
    // s->hash1 = s->hash1 << 1;
    return s;
}

/*cdoc State IoState_removeSymbol_(self, s)
Drops an IoSymbol from the intern table, called by the collector's
willFree path when a Symbol is about to be swept. Without this the
symbol table would keep pointers to freed memory and any future
intern of the same bytes would return a dangling IoSymbol.
*/
void IoState_removeSymbol_(IoState *self, IoSymbol *s) {
    CHash_removeKey_(self->symbols, IoSeq_rawUArray(s));
}
