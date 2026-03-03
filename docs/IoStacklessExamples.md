# Io Stackless VM Examples

The stackless branch replaces Io's recursive C-stack evaluator with a heap-allocated frame-based iterative evaluator. This enables portable coroutines, tail call optimization, frame introspection, and robust exception handling — all without platform-specific assembly, `setjmp`/`longjmp`, or `ucontext`.

---

## Tail Call Optimization

The stackless evaluator has two TCO mechanisms that keep frame stacks flat for recursive patterns.

### Direct Tail Recursion

When a block call is the last message in a block body, the frame is reused instead of pushing a new one:

```io
countdown := method(n,
    if(n <= 0, return n)
    countdown(n - 1)
)
countdown(100000)  // no stack overflow
```

### TCO Through If Branches

When `if()` is the last message in a chain, the selected branch evaluates in-place. This is the idiomatic Io recursion pattern:

```io
factorial := method(n, acc,
    if(n <= 1, acc, factorial(n - 1, n * acc))
)
factorial(100000, 1)  // no stack overflow

sumAcc := method(n, acc,
    if(n <= 0, return acc)
    sumAcc(n - 1, acc + n)
)
sumAcc(100000, 0)  // => 5000050000
```

### Non-Tail Recursion

Even without TCO, recursion is bounded by heap rather than C stack depth:

```io
sumTo := method(n, if(n <= 0, 0, n + sumTo(n - 1)))
sumTo(100)  // => 5050, uses heap frames
```

---

## Coroutines

Coroutines work by saving and restoring the frame pointer — no C stack switching. A suspended coroutine's entire state is a single pointer to its saved frame chain. Switching is O(1).

### Async Dispatch (@@)

The `@@` operator dispatches a message to a new coroutine:

```io
o := Object clone
o work := method(
    for(i, 1, 5, i println; yield)
)
o @@work
for(i, 1, 5, yield)
```

### Futures (@)

The `@` operator returns a future that resolves when the coroutine completes:

```io
obj := Object clone
obj double := method(v, v * 2)
future := obj @double(2)
future println  // => 4
```

### Manual Coroutine Control

Create and resume coroutines directly:

```io
c := Coroutine clone
c setRunTarget(Lobby)
c setRunLocals(Lobby)
c setRunMessage(message("from coro" println))
c resume
```

### Cooperative Scheduling

Coroutines yield control explicitly with `yield` and `pause`:

```io
o := Object clone
o s := Sequence clone
o l := method(
    j := 1
    loop(
        s appendSeq("a", j asString, ".")
        if(j % 2 == 0, pause)
        j = j + 1
    )
)

o @@l
for(i, 1, 4,
    yield
    o s appendSeq("b", i asString, ".")
    if(i == 2, o actorCoroutine recentInChain resumeLater)
)
// o s => "a1.a2.b1.b2.a3.a4.b3.b4."
```

### Scheduler

The `Scheduler` manages a single shared queue called `yieldingCoros` — the list of coroutines that are ready to run. All coroutine switching goes through this queue.

**yield** appends the current coroutine to the back of the queue, pops the first one off, and resumes it. If the queue is empty, yield is a no-op. If the current coroutine is the only one in the queue, it's also a no-op:

```io
// Coroutine yield (simplified):
yield := method(
    if(yieldingCoros isEmpty, return)
    yieldingCoros append(self)        // put ourselves at the back
    next := yieldingCoros removeFirst // take the next one off the front
    if(next == self, return)          // we're the only one, nothing to do
    next resume                       // switch to it
)
```

**pause** removes the current coroutine from the queue entirely and resumes the next one. The paused coroutine won't run again until something calls `resume` or `resumeLater` on it:

```io
// Coroutine pause (simplified):
pause := method(
    yieldingCoros remove(self)
    next := yieldingCoros removeFirst
    if(next, next resume,
        Exception raise("Scheduler: nothing left to resume")
    )
)
```

**resumeLater** puts a coroutine at the front of the queue without switching to it — it will be the next one to run when the current coroutine yields:

```io
someCoroutine resumeLater  // insert at front of yieldingCoros
```

The scheduler loop waits for all coroutines to finish:

```io
Scheduler waitForCorosToComplete  // yields until yieldingCoros is empty
```

### Waiting

`Object wait(s)` is a cooperative sleep — if other coroutines are in the queue, it yields in a loop until the deadline passes. If no other coroutines exist, it falls back to `System sleep`:

```io
// cooperative wait — other coroutines run while we wait
wait(0.5)

// equivalent to:
endDate := Date clone now + Duration clone setSeconds(0.5)
loop(endDate isPast ifTrue(break); yield)
```

### Futures and Waiting on Results

The `@` operator returns a `FutureProxy`. When you send any message to the proxy, the calling coroutine pauses until the result is ready. The actor's coroutine calls `setResult` when done, which `resumeLater`s all waiting coroutines:

```io
obj := Object clone do(
    compute := method(n, n * n)
)

result := obj @compute(7)
// result is a FutureProxy — calling coroutine keeps running
doOtherWork
result println  // pauses here until compute finishes, then prints 49
```

Under the hood:

```io
// Future waitOnResult (simplified):
waitOnResult := method(
    waitingCoros append(Scheduler currentCoroutine)
    Scheduler currentCoroutine pause  // removes us from yieldingCoros
)

// Future setResult — called when the actor finishes:
setResult := method(r,
    proxy _become(r)                          // proxy becomes the real value
    waitingCoros foreach(resumeLater)          // wake up everyone who was waiting
)
```

This is the general pattern for all async operations in Io. A coroutine waiting on an async operation (socket read, file I/O, timer, etc.) is **not in the `yieldingCoros` queue at all** — `pause` removed it. The coroutine only exists as a reference in the operation's waiting list. It won't be scheduled until the operation completes and calls `resumeLater`, which puts it back at the front of `yieldingCoros`.

For example, a socket read looks like:

```io
// Inside a Socket read method (conceptual):
streamReadNextChunk := method(
    waitingCoros append(Scheduler currentCoroutine)
    Scheduler currentCoroutine pause   // off the scheduler entirely
    // ... execution resumes here when data arrives ...
    readBuffer
)

// When the event loop detects data ready on the socket:
onReadable := method(
    waitingCoros foreach(resumeLater)   // back on the scheduler
    waitingCoros empty
)
```

The VM's `IoState_activeCoroCallback` hook (in `IoState_callbacks.c`) lets an external event loop (libevent, kqueue, epoll, etc.) drive the scheduler — it gets notified when the coroutine count changes so it can integrate polling with coroutine switching.

### Actor Pattern

Objects become actors with `@` and `@@`. Each actor gets its own coroutine and message queue, processing messages one at a time with `yield` between each:

```io
Database := Object clone do(
    store := method(key, value,
        // ... store data ...
        "stored #{key}" interpolate println
    )
)

// Fire-and-forget (@@) — returns nil
Database @@store("a", 1)
Database @@store("b", 2)

// Future (@) — returns a proxy that blocks on access
result := Database @store("c", 3)
result println  // pauses here until store completes
```

---

## Frame Introspection

Live execution frames are exposed to Io code for debugging and metaprogramming.

### Walking the Frame Stack

```io
f := Coroutine currentCoroutine currentFrame
while(f != nil,
    f description println
    f = f parent
)
```

### Inspecting Frame Properties

Each frame exposes its execution state:

```io
f := Coroutine currentCoroutine currentFrame
f state println       // frame state machine state (e.g., "activate")
f depth println       // distance from bottom of frame stack
f target println      // receiver object (self)
f locals println      // enclosing scope
f result println      // accumulated result
f message println     // current message being evaluated
f call println        // call introspection object (in methods)
f blockLocals println // block's local scope (in block activations)
```

### Programmatic Stack Traces

```io
stackTrace := method(
    frames := list
    f := Coroutine currentCoroutine currentFrame
    while(f != nil,
        frames append(f description)
        f = f parent
    )
    frames
)
```

---

## Exception Handling

Exceptions use the frame unwinding mechanism — no `longjmp`/`setjmp`. Both C-level errors and Io-level `Exception raise` set `errorRaised` and the eval loop unwinds frames.

### Try / Catch

```io
e := try(Exception raise("boom"))
e error println  // => "boom"
"execution continues" println
```

### Exception Pass (Re-raise)

```io
e := try(
    try(Exception raise("inner")) pass
)
e error println  // => "inner"
```

### C-Level Errors

C-level errors (type mismatches, index out of bounds, etc.) are caught the same way:

```io
e := try(1 unknownMethod)
e error println  // => "Object does not respond to 'unknownMethod'"
```
