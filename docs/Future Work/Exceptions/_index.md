# Exceptions

Design notes on the current exception system and proposed improvements.

## Current Implementation

### try/catch (non-resumable)

Standard exception handling. `try(code)` runs code in a child coroutine. If an exception is raised, it's captured on the coroutine and returned. `catch` filters by exception type. `pass` re-raises.

```io
e := try(Exception raise("boom"))
e catch(Exception, writeln(e error))
```

### signal/withHandler (resumable)

Added in the `stackless` branch. `withHandler` installs a handler on the coroutine's handler stack, evaluates the body, then removes the handler. `signal` walks the handler stack to find a matching handler and calls it as a subroutine — the signaler's frames remain on the stack.

```io
result := withHandler(Exception, block(exc, resume,
    "default value"
),
    Exception signal("something went wrong")
)
// result is "default value"
```

The handler runs as a normal block activation with its own frame. Whatever the handler returns becomes the return value of `signal()` at the call site. No coroutine switch, no frame manipulation, no `callcc`.

`_Resumption` is a placeholder object whose `invoke(v)` returns `v`. It exists for API consistency but is functionally the identity.

## Comparison with Other Languages

### Common Lisp Condition System

CL separates three concerns:

1. **Signaling** — low-level code detects a problem and signals a condition
2. **Handling** — high-level code decides on a recovery policy
3. **Restarting** — low-level code offers named recovery strategies

```lisp
;; Low-level: offers restarts, doesn't choose
(defun parse-entry (line)
  (restart-case (actually-parse line)
    (skip-entry () nil)
    (use-value (v) v)))

;; High-level: chooses policy, doesn't know recovery details
(handler-bind ((malformed-entry
                (lambda (c) (invoke-restart 'skip-entry))))
  (parse-log-file stream))
```

Key features Io lacks:
- **Restarts**: named recovery points established by the signaler. The handler picks one without knowing recovery details. This decouples policy from mechanism.
- **Decline**: a `handler-bind` handler can return normally without invoking a restart, and the search continues to the next handler. In Io, the handler's return value is always used.
- **Interactive debugging**: unhandled conditions present available restarts to the developer in the debugger. SLIME's debugger lets you inspect the live stack, fix data, and resume — the program continues as if the error never happened.

### Smalltalk (on:do:)

Handler receives the exception object with a rich protocol:
- `ex resume: value` — resume at signal site with value (non-local return from handler)
- `ex return: value` — return from the protected block
- `ex retry` — re-run the protected block
- `ex pass` — delegate to outer handler
- `ex outer` — invoke outer handler, then resume

More expressive than Io's current system but lacks CL's restart separation.

## Possible Directions

### 1. Add Restarts (CL-style)

The most impactful addition. Restarts let the signaler offer recovery strategies without choosing one, and the handler choose a strategy without knowing recovery details.

Possible Io API:

```io
// Low-level code establishes restarts
parseEntry := method(line,
    withRestarts(
        list(
            Restart clone setName("skipEntry") setAction(block(nil)),
            Restart clone setName("useValue") setAction(block(v, v))
        ),
        actuallyParse(line)
    )
)

// High-level code picks a policy
withHandler(MalformedEntry, block(exc, resume,
    invokeRestart("skipEntry")
),
    parseLogFile(stream)
)
```

Implementation: a restart registry (List) on the Coroutine, similar to `handlerStack`. `withRestarts` pushes entries, `invokeRestart` walks the registry. Pure Io-level code, no VM changes needed.

### 2. Handler Decline

Allow a handler to decline (pass to next handler) instead of always producing a value. Could use a sentinel:

```io
withHandler(Exception, block(exc, resume,
    if(exc error containsSeq("fatal"),
        decline  // search continues to next handler
    ,
        "recovered"
    )
),
    body
)
```

### 3. Richer Exception Protocol (Smalltalk-style)

Add methods to the exception object for `retry`, `return(value)`, `outer`. These would use the eval loop's existing stop-status mechanism or handler stack walking.

### 4. Interactive Restart Selection in REPL

When an unhandled signal reaches the top level with available restarts, present them to the user:

```
Error: Malformed entry at line 42
Available restarts:
  0: [skipEntry] Skip this entry
  1: [useValue]  Supply a replacement value
  2: [abort]     Abort
Pick a restart:
```

This requires restarts (direction 1) and REPL integration.

### Recommendation

Start with **restarts** (direction 1). They provide the biggest leverage — decoupling error policy from recovery mechanism — and map naturally to Io's prototype model. A Restart is just an Object with `name` and `action` slots. The handler/restart registries are Lists on the Coroutine. No VM changes, no new primitives, just Io-level code building on `withHandler`.

Decline (direction 2) is a small addition on top. Interactive selection (direction 4) is the long-term payoff but requires restarts first.
