# Phase 5: Compacting Collector

The current Baker treadmill collector works but fragments memory over time. A compacting collector would reduce memory usage for long-running browser sessions.

The codebase is **moderately ready** (7/10).

## Design

**New GC hook needed**: A `remap` callback alongside the existing `mark` and `free` callbacks. Where `mark` visits references, `remap` must be able to **write back** updated addresses to pointer fields. The cleanest implementation is a single `walkRefs` function per primitive type that accepts a visitor, with both `mark` and `remap` implemented as thin wrappers over it:

```c
void IoObject_walkRefs(IoObject *self, IoRefVisitor *visitor, void *ctx);
void IoObject_mark(IoObject *self);   // wraps walkRefs with mark visitor
void IoObject_remap(IoObject *self);  // wraps walkRefs with remap visitor
```

Only a small number of Io primitives contain object references and need `walkRefs` — `IoObject`, `IoList`, `IoMessage`, `IoCoroutine`. Leaf primitives (`IoNumber`, `IoSeq`, `IoDate`, `IoFile`) contain no object references and need only a `memmove` during compaction.

**50% boundary policy**: Compaction only moves objects **above the 50% memory mark** to lower addresses. Objects below 50% are never moved. This preserves JIT optimization assumptions for hot long-lived objects in the lower half, while allowing the upper half to act as a nursery zone. The midpoint should be dynamic:

```c
size_t midpoint = max(wasmMemorySize() / 2, liveObjectBytes() * 1.2);
```

**Recycling interaction**: `libgarbagecollector` has a `COLLECTOR_RECYCLE_FREED` mechanism (currently disabled via a commented-out `#define`). When compaction is added, recycling should only operate on freed slots **above the 50% boundary**, keeping the lower half stable and dense.

**Compaction trigger**: Only fire under memory pressure, before calling `memory.grow`. Sequence: compact upper half -> if still above 80% full -> grow. This makes compaction a pressure relief valve rather than a routine operation.

**JIT consideration**: Compaction invalidates JIT-learned access patterns. The 50% policy mitigates this by never moving JIT-hot lower-half objects. Compaction should be infrequent and ideally triggered between activity bursts.

**Treadmill color as hotness proxy**: The Baker Treadmill's off-white segment (tenured/long-lived objects) naturally identifies hot objects without any separate heat tracking. During compaction, off-white objects in the lower half should be left in place.

## Types needing walkRefs

| Type | Complexity | Key references |
|------|-----------|----------------|
| IoObject | Medium | slots (PHash k,v), protos[] (**interior pointer**), tag's walkRefs |
| IoMessage | Medium | name, args, next, label, cachedResult, **inline cache value/context** |
| IoEvalFrame | High | 15+ fields + control flow union (60-line switch) |
| IoCoroutine | High | ioStack, frameStack chain, pooled frames/blockLocals/calls |
| IoList | Easy | iterate items |
| IoMap | Easy | iterate PHash k,v |
| IoBlock | Easy | message, scope, argNames |
| IoCall | Easy | 6 pointer fields |
| IoContinuation | Medium | capturedLocals, capturedFrame chain |
| IoCFunction | Easy | uniqueName |
| IoFile | Easy | path, mode |
| IoDirectory | Easy | path |
| IoWeakLink | Easy | link (needs remap despite no mark func) |

## Known complications

1. **Interior pointer in protos**: `IoObject_justAlloc` allocates `IoObjectData + protos[]` as one block. Protos pointer is `data + 1`. Compaction must either:
   - Allocate protos separately (simpler, more fragments), or
   - Remap interior pointer via offset calculation

2. **Inline cache**: IoMessage caches slot lookups. Compaction must invalidate all caches or remap cached pointers.

3. **argValues interior pointer**: IoEvalFrame's `argValues` may point to embedded `inlineArgs[4]` buffer.

4. **COLLECTOR_RECYCLE_FREED**: Currently disabled. Keep disabled during compaction work.

## Recommendation

Don't implement compaction during the WASM migration. But avoid making it harder:
- Don't add new interior pointer patterns
- Don't add new global caches of IoObject pointers
- Keep mark functions as the single source of truth for reference traversal
