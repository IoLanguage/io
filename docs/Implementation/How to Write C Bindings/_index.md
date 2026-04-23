# How to Write C Bindings

How to write Io-visible primitives in C — the macros, the argument-extraction API, proto registration, and GC-safe slot updates.

## Method Signature

Every C-implemented slot has the same three arguments: the receiver, the locals object of the caller, and the unevaluated message node. The `IO_METHOD` macro (defined in `IoObject_struct.h`) writes the prototype for you:

```c
#define IO_METHOD(CLASS, NAME) \
    IoObject *CLASS##_##NAME(CLASS *self, IoObject *locals, IoMessage *m)
```

So `IO_METHOD(IoList, at)` expands to:

```c
IoObject *IoList_at(IoList *self, IoObject *locals, IoMessage *m)
```

Three things to remember about the arguments:

- **`self`** — the receiver. Its primitive data is reached through `DATA(self)`, a thin macro over `IoObject_dataPointer(self)`.
- **`locals`** — the scope of the calling method or block. You need it to evaluate the message's arguments, because each argument is itself a message that has to run in the caller's scope.
- **`m`** — the live `IoMessage` node. Its name, argument messages, and next pointer are all accessible for introspection.

## Extracting Arguments

You never reach into `m` directly. The VM provides `IoMessage_locals_*ArgAt_` helpers that take `(m, locals, index)`, evaluate the argument message in the caller's scope, type-check the result, and raise an Io-level exception on a mismatch. The common ones:

| Call | Returns |
|---|---|
| `IoMessage_locals_intArgAt_` | `int` |
| `IoMessage_locals_longArgAt_` | `long` |
| `IoMessage_locals_sizetArgAt_` | `size_t` |
| `IoMessage_locals_doubleArgAt_` | `double` |
| `IoMessage_locals_floatArgAt_` | `float` |
| `IoMessage_locals_boolArgAt_` | `int` |
| `IoMessage_locals_numberArgAt_` | `IoNumber *` |
| `IoMessage_locals_symbolArgAt_` | `IoSymbol *` |
| `IoMessage_locals_seqArgAt_` | `IoSeq *` |
| `IoMessage_locals_cStringArgAt_` | `const char *` |
| `IoMessage_locals_blockArgAt_` | `IoBlock *` |
| `IoMessage_locals_listArgAt_` | `IoList *` |
| `IoMessage_locals_mapArgAt_` | `IoMap *` |
| `IoMessage_locals_messageArgAt_` | `IoMessage *` |
| `IoMessage_locals_dateArgAt_` | `IoDate *` |

Arity and raw access:

- `IoMessage_assertArgCount_receiver_(m, n, self)` — raise if the caller passed fewer than `n` arguments.
- `IoMessage_argCount(m)` — number of argument messages.
- `IoMessage_locals_valueArgAt_(m, locals, i)` — the untyped result of evaluating arg `i`.

### Deferred arguments

Control-flow-style methods sometimes want the unevaluated argument message instead of its value (so they can choose whether, when, or how many times to evaluate it). Grab it from `m` directly with `IoMessage_rawArgAt_(m, i)` and evaluate it later with `IoMessage_locals_performOn_(arg, locals, target)`.

## Constructing Results

Return values are Io objects. A handful of macros produce the common ones:

| Macro | Produces |
|---|---|
| `IONUMBER(x)` | `IoNumber` wrapping a C `double` |
| `IOBOOL(self, b)` | the shared `true` / `false` singletons |
| `IONIL(self)` | the shared `nil` singleton |
| `IOSYMBOL(s)` | interned `IoSeq` symbol from a C string |
| `IOSEQ(bytes, len)` | mutable `IoSeq` |
| `IOLIST(state, ...)` | literal `IoList` |

Returning `self` is fine — chainable setters do it constantly.

## Registering a Proto

Primitives install themselves at VM init with a `{name, cfunc}` table and a single call. The pattern from `IoList.c`:

```c
IoList *IoList_proto(void *state) {
    IoMethodTable methodTable[] = {
        {"at",     IoList_at},
        {"atPut",  IoList_atPut},
        {"append", IoList_append},
        {"size",   IoList_size},
        {NULL, NULL},
    };

    IoObject *self = IoObject_new(state);
    IoObject_tag_(self, IoList_newTag(state));
    IoObject_setDataPointer_(self, List_new());
    IoState_registerProtoWithId_((IoState *)state, self, protoId);
    IoObject_addMethodTable_(self, methodTable);
    return self;
}
```

Steps:

1. **Allocate** a fresh object and give it your tag. The tag (from `IoList_newTag`) carries the clone/mark/free/compare function pointers the GC and `clone` will call.
2. **Stash primitive state** in the data pointer. For `IoList` that's a basekit `List`; for your type it's whatever C struct the slot methods will read through `DATA(self)`.
3. **Register** the proto under a unique id (usually `static IoTag *protoId = "IoList";`). Other C code reaches this proto with `IoState_protoWithId_`.
4. **Install** the method table. Every `{name, cfunc}` pair becomes a slot on the proto.

A matching `IoList_rawClone` is registered as the tag's clone function so each clone gets its own primitive data (otherwise mutation of one instance would leak into the proto).

Finally, call your `IoList_proto` from `IoState.c`'s init so the proto is installed before Io code runs.

## GC Safety

The collector runs incrementally, so any time you store an `IoObject *` into another live object you must tell the collector you did so. Use `IOREF`:

```c
IoObject_setSlot_to_(self, IOSYMBOL("name"), IOREF(newValue));
```

`IOREF` is the write barrier. Miss it and a collection between the store and the next safe point can reclaim the value you just installed. Every slot helper (`setSlot_to_`, `updateSlot_to_`, protos, listeners) takes `IOREF`'d values.

Local `IoObject *` variables inside a method are fine without `IOREF` — they're already reachable through the frame chain that the collector walks.

## Raising Errors

Io-level exceptions are raised from C with `IoState_error_`:

```c
if (index < 0 || index >= List_size(DATA(self))) {
    IoState_error_(IOSTATE, m, "index %d out of range", index);
    return IONIL(self);  // unreachable in practice, but keeps compilers happy
}
```

`IoState_error_` sets `state->errorRaised = 1` and the iterative eval loop unwinds frames until an `Io`-level `try`/`catch` runs. There is no longjmp — just flag-and-return. That's why helpers that hit an error still return a value; the caller propagates by checking `errorRaised` or, more commonly, the eval loop handles it a step later.

The argument-extraction helpers do this for you: `IoMessage_locals_intArgAt_` raises a type error and returns `0` if the argument isn't a number.

## A Complete Example

```c
// A pretend IoPoint with x and y doubles stored in data.ptr.

typedef struct { double x, y; } PointData;

static IoTag *protoId = "IoPoint";

IO_METHOD(IoPoint, distanceTo) {
    /*doc Point distanceTo(other) Euclidean distance to other Point. */

    IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);
    IOASSERT(ISPOINT(other), "argument must be a Point");

    PointData *a = DATA(self);
    PointData *b = DATA(other);
    double dx = a->x - b->x;
    double dy = a->y - b->y;
    return IONUMBER(sqrt(dx*dx + dy*dy));
}

IoPoint *IoPoint_proto(void *state) {
    IoMethodTable methodTable[] = {
        {"distanceTo", IoPoint_distanceTo},
        {NULL, NULL},
    };

    IoObject *self = IoObject_new(state);
    IoObject_tag_(self, IoPoint_newTag(state));
    IoObject_setDataPointer_(self, io_calloc(1, sizeof(PointData)));
    IoState_registerProtoWithId_((IoState *)state, self, protoId);
    IoObject_addMethodTable_(self, methodTable);
    return self;
}
```

That's the shape of every primitive in `libs/iovm/source/` — `IoNumber`, `IoSeq`, `IoList`, `IoMap`, `IoBlock`, and the rest. Read any of them as a reference when implementing your own.
