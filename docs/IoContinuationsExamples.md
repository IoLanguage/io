# Io Continuations Examples

First-class continuations are created with `callcc`, which calls a block with the current continuation as its argument. The continuation captures a deep copy of the execution state (frame stack) at the point of the `callcc` call.

## Escape Continuation (Early Return)

The most common pattern — exit a nested computation early:

```io
result := callcc(block(escape,
    list(1, 2, 3, 4, 5) foreach(v,
        if(v == 3,
            escape invoke("found 3!")
        )
    )
    "not found"
))
result println  // => "found 3!"
```

When `escape invoke("found 3!")` is called, execution jumps back to the `callcc` site and `result` receives `"found 3!"` instead of `"not found"`.

## Exception-Like Non-Local Exit

Use continuations to bail out of a function on error without raising an exception:

```io
safeDiv := method(a, b,
    callcc(block(bail,
        if(b == 0,
            bail invoke("division by zero")
        )
        a / b
    ))
)

safeDiv(10, 2) println   // => 5
safeDiv(10, 0) println   // => "division by zero"
```

## Deferred Invocation

Continuations can be saved and invoked after the `callcc` block returns normally:

```io
savedCont := nil
value := callcc(block(cont,
    savedCont = cont
    1
))
("value: " .. value) println
if(value == 1,
    savedCont invoke(42)
)
// Prints:
//   value: 1
//   value: 42
```

## Multi-Shot (Generator Pattern)

Continuations are one-shot by default, but `copy` creates a fresh copy that can be invoked independently. This enables generator-like patterns:

```io
savedCont := nil
value := callcc(block(cont,
    savedCont = cont
    1
))
value println
if(value < 3, savedCont copy invoke(value + 1))
"done" println
// Prints: 1, 2, 3, done
```

Each `copy invoke(n)` creates a fresh continuation and invokes it, resuming from the `callcc` site with a new value.

## Serialization and Inspection

Continuations can be serialized to a Map and restored with `fromMap`:

```io
callcc(block(cont,
    // Inspect the continuation
    ("Frame count: " .. cont frameCount) println
    ("States: " .. cont frameStates) println
    ("Messages: " .. cont frameMessages) println

    // Serialize to a Map
    m := cont asMap
    m keys foreach(k, (k .. ": " .. m at(k)) println)

    // Round-trip: restore from serialized form
    cont2 := cont clone fromMap(m)
    ("Restored frame count: " .. cont2 frameCount) println
    ("Restored states: " .. cont2 frameStates) println
))
```

The `asMap` representation includes frame states, message code strings, control flow state, and argument values. Message trees are reparsed from their code string representations during `fromMap`, so the round-trip preserves execution structure. Object identity (target/locals) is set to Lobby as a placeholder since it cannot be reconstructed from type names alone.

## Backtracking (amb Operator)

Multi-shot continuations via `copy` can implement backtracking search:

```io
failures := list

amb := method(choices,
    callcc(block(cont,
        choices foreach(choice,
            callcc(block(fail,
                failures append(fail)
                cont copy invoke(choice)
            ))
        )
        Exception raise("amb: no more choices")
    ))
)
```

Each call to `amb` returns the first choice. If a later constraint fails, invoking the most recent failure continuation (via `copy`) backtracks and tries the next choice.

## Continuation API

| Method | Description |
|--------|-------------|
| `invoke(value)` | Restore captured frames, return value at callcc site |
| `copy` | Deep-copy the frame chain (enables multi-shot use) |
| `isInvoked` | Returns true if this continuation has been invoked |
| `frameCount` | Number of captured frames |
| `frameStates` | List of state names per frame |
| `frameMessages` | List of current messages per frame |
| `asMap` / `fromMap` | Serialize/deserialize continuation state |

## Resumable Exceptions

The resumable exception system builds on the continuation infrastructure. `signal` is the resumable counterpart to `raise` — a handler can inspect the exception and supply a replacement value, resuming execution at the signal site.

### Basic Signal and Resume

```io
result := withHandler(Exception,
    block(e, resume, resume invoke(42)),
    Exception signal("need value") + 1
)
result println  // => 43
```

`Exception signal("need value")` looks for a matching handler. The handler receives `(exception, resume)` and calls `resume invoke(42)`, which makes `signal` return 42. Then `42 + 1` evaluates to 43.

### Auto-Resume (Handler Return Value)

If the handler simply returns a value (without explicit `resume invoke`), that value becomes signal's return value:

```io
result := withHandler(Exception,
    block(e, resume, 42),
    Exception signal("need value") + 1
)
result println  // => 43
```

### Re-Raise from Handler

A handler can decline to resume and re-raise the exception as non-resumable:

```io
e := try(
    withHandler(Exception,
        block(exc, resume, exc pass),
        Exception signal("will re-raise")
    )
)
e error println  // => "will re-raise"
```

### Custom Exception Types

Handlers match by prototype chain using `isKindOf`:

```io
MyError := Exception clone do(type := "MyError")
result := withHandler(MyError,
    block(exc, resume, resume invoke("handled")),
    MyError signal("custom error")
)
result println  // => "handled"
```

A handler for `MyError` won't catch a plain `Exception signal`. A handler for `Exception` catches everything (since all exception types descend from Exception).

### Nested Handlers

Handlers form a stack — inner handlers take priority:

```io
result := withHandler(Exception,
    block(e, resume, resume invoke("outer")),
    withHandler(Exception,
        block(e, resume, resume invoke("inner")),
        Exception signal("test")
    )
)
result println  // => "inner"
```

### Multiple Signals in One Body

A handler can handle multiple signals from the same body. It's called once per signal:

```io
count := 0
result := withHandler(Exception,
    block(e, resume,
        count = count + 1
        resume invoke(count)
    ),
    a := Exception signal("first")
    b := Exception signal("second")
    list(a, b)
)
result println  // => list(1, 2)
```

### Signal Inside Try

Handlers installed outside a `try` are visible inside it (handler lookup walks the `parentCoroutine` chain):

```io
result := withHandler(Exception,
    block(e, resume, resume invoke(99)),
    e := try(Exception signal("inside try"))
    if(e, "exception", "no exception")
)
result println  // => "no exception"
```

### Resumable Exception API

| Method | Description |
|--------|-------------|
| `Exception signal(error)` | Resumable raise — finds handler, calls it, returns handler's result |
| `withHandler(proto, handler, body)` | Install handler for exceptions matching `proto`, run body |
| `resume invoke(value)` | In handler: make signal return `value` at signal site |
| `exception pass` | In handler: re-raise as non-resumable exception |

**Coexistence with raise:** `raise` is unchanged and always non-resumable. `signal` with no matching handler falls back to `raise`.

## Notes

- Continuations are **one-shot by default**. Use `copy` to create a fresh continuation for multi-shot patterns.
- `callcc` captures a deep copy of the frame stack, so continuations are independent of the original execution context and can be invoked after the `callcc` block returns normally.
- `callcc` requires the iterative evaluator (the stackless frame-based eval loop). It will raise an error if called during bootstrap recursive evaluation.
