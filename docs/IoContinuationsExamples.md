# Io Continuations Examples

First-class continuations are created with `callcc`, which calls a block with the current continuation as its argument. The continuation captures the full execution state (frame stack) at the point of the `callcc` call.

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

## Generator Pattern (Multi-Shot)

A continuation marked `setMultiShot` can be invoked more than once, enabling generator-like patterns:

```io
yield := nil
value := callcc(block(cont,
    cont setMultiShot
    yield = cont
    1  // first yielded value
))
// Each time yield invoke(n) is called, execution resumes
// here with value set to n
value println
```

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

Multi-shot continuations can implement backtracking search:

```io
failures := list

amb := method(choices,
    callcc(block(cont,
        cont setMultiShot
        choices foreach(choice,
            callcc(block(fail,
                failures append(fail)
                cont invoke(choice)
            ))
        )
        Exception raise("amb: no more choices")
    ))
)
```

Each call to `amb` returns the first choice. If a later constraint fails, invoking the most recent failure continuation backtracks and tries the next choice.

## Notes

- Continuations are **one-shot by default**. Call `setMultiShot` before invoking if you need to invoke more than once.
- `callcc` requires the iterative evaluator (the stackless frame-based eval loop). It will raise an error if called during bootstrap recursive evaluation.
- Continuations capture a deep copy of the frame stack, so they are independent of the original execution context.
