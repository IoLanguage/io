# Overview

A tour of the Io VM source tree — object representation, the message tree, the evaluator, the standard library loader, and the garbage collector.

## Layout

The VM lives under `libs/iovm/` and is about 25k lines of portable C, with no threading, no assembly, and no platform-specific syscalls beyond what WASI provides.

```
libs/iovm/source/     Core VM (C)
libs/iovm/io/         Standard library (.io)
libs/iovm/tests/      C test files
tools/source/main.c   REPL / CLI entry point
```

The public C API is in `IoState.h`, `IoObject.h`, `IoMessage.h`, and the per-primitive headers (`IoNumber.h`, `IoSeq.h`, `IoList.h`, `IoMap.h`, `IoBlock.h`, `IoCoroutine.h`, etc.). Every file is prefixed `Io` so the C namespace stays tidy when embedded.

## Objects

Every Io value is a `CollectorMarker` — the header that the collector uses to link live objects on its gray/black/white lists. The marker's `object` field points to an `IoObjectData`:

```c
struct IoObjectData {
    unsigned int markerCount;
    union { void *ptr; double d; uint32_t ui32; } data;  // primitive payload
    IoTag *tag;                                          // vtable
    PHash *slots;                                        // cuckoo hash
    List *listeners;
    IoObject **protos;                                   // parent list
    /* ... flag bits: isActivatable, isLocals, isSymbol, ... */
};
```

A few things to notice:

- **No classes.** A `tag` names the primitive kind (`Number`, `Sequence`, `List`, …) and carries function pointers for clone, mark, free, and compare. Behavior lives in slots, not in the tag.
- **Slots are a PHash** — a cuckoo hash table keyed by interned symbol pointers. Lookup is a single hash and an identity compare; no string compare, no collision chain.
- **Protos are a plain C array.** Lookup walks the array depth-first with loop detection via the `hasDoneLookup` bit. Multiple inheritance falls out naturally.
- **The `data` union holds the primitive payload.** A Number stores its `double` inline; a List's `ptr` points at a basekit `List`; a Block's `ptr` points at the compiled message tree. Everything else — identity, identity-compare, GC — goes through the same header.

Assignment operators desugar to slot sends: `:=` → `setSlot`, `=` → `updateSlot`, `::=` → `newSlot`. Slot creation is a write; slot lookup is the proto walk.

## The Message Tree

Parsing produces a tree of `IoMessage` nodes, each holding a `name` symbol, an argument list of messages, and a `next` pointer to the next message in the chain. Operators are ordinary messages at this stage.

Then `IoMessage_opShuffle.c` rewrites the tree by operator precedence using the table in `libs/iovm/io/OperatorTable.io`. After shuffling, an expression like `1 + 2 × 3` has become `1 +(2 ×(3))` — the same tree shape you'd get from an explicit method call.

Because messages are first-class objects, the shuffle is just tree rewriting on live data. Programs can read, rewrite, and evaluate messages the same way. This is what makes `if`, `while`, `for`, and `method` implementable in Io itself rather than as compiler built-ins.

## Evaluation

`IoState_iterative.c` implements a single `while(1)` eval loop. Each iteration pulls the current heap-allocated `IoEvalFrame` off the state's `currentFrame`, steps its state machine one click, and loops. There is no C recursion for message evaluation; argument evaluation, block activation, control flow, and coroutine switching all happen by pushing frames and transitioning states.

A frame carries everything a recursive evaluator would keep on the C stack:

```
target        the receiver (self)
locals        the enclosing scope
message       the current message being evaluated
argN          pre-evaluated arguments
result        accumulated chain result
state         state-machine enum
parent        previous frame
```

This layout buys four properties:

1. **First-class continuations.** `callcc` captures the current frame chain as an ordinary Io object. Invoking it later just swaps `currentFrame`. (Disabled by default behind `-DIO_CALLCC`.)
2. **Portable coroutines.** A suspended coroutine is a saved frame pointer. Switching is pointer assignment — no setjmp/longjmp, no ucontext, no fibers. The same C code runs on every target, including WASM.
3. **Tail-call optimization.** When a call is the last message in a block body, the eval loop reuses the current frame instead of pushing a new one.
4. **Clean exception unwinding.** `IoState_error_` sets `state->errorRaised` and the loop pops frames until a handler is found. No longjmp hopping over C code that expected to run cleanup.

`IoMessage_locals_performOn_` (the old recursive evaluator) still exists for bootstrap. Once the first eval loop starts, it redirects to the iterative path.

## Special Forms

Most messages pre-evaluate their arguments before the receiver's method runs. A handful must not, because they implement control flow or introspect the unevaluated message itself:

```
if  while  loop  for  callcc  method  block
foreach  reverseForeach  foreachLine
```

The iterative evaluator checks for these by name in two places in `IoState_iterative.c` and skips pre-evaluation. The method then evaluates its argument messages explicitly with the evaluator APIs.

## The Standard Library

The files in `libs/iovm/io/` are loaded in the explicit order listed by the `IO_FILES` variable in the root `Makefile`:

```
List_bootstrap.io, Object_bootstrap.io, OperatorTable.io,
Object.io, List.io, Exception.io,
<alphabetical core>,
CLI.io, Importer.io
```

Order matters — the bootstrap files install the minimum slots needed to load everything else. `make regenerate` runs `io2c` to compile these files into string literals inside `libs/iovm/source/IoVMInit.c`, which the VM evaluates at startup. That means the shipped binary carries its own standard library; there is no filesystem lookup at init.

## Garbage Collector

The collector is an incremental, tri-color, mark-and-sweep design living under `libs/garbagecollector/`. Roots are the lobby, the active coroutine's frame chain, and any object the VM has pinned with `IoState_retain_`. Write barriers (`IoObject_shouldMark`, invoked from `IOREF`) keep the gray set consistent when mutators store new references into already-scanned objects.

Because frames are ordinary heap objects, the live stack is itself reachable through `state->currentFrame`. Stack traces, debuggers, and serializers all walk the same graph the collector marks.

## Coroutines

A coroutine is just a saved `currentFrame` plus a small bit of bookkeeping. `Coroutine resume` sets the VM's `currentFrame` to the coroutine's saved pointer; the eval loop picks up where it left off. `yield` and `pause` cooperate with `Scheduler` to pick the next coroutine.

See [Stackless](../../Stackless/index.html) for a deeper treatment of the evaluator, the scheduler, the actor pattern, and the performance characteristics of the frame machine.
