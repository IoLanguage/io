# Style Guide

How to write idiomatic Io — naming, formatting, comments, and the patterns used throughout the standard library.

## Naming

Io distinguishes *objects* (prototypes) from *slots on objects* (methods and values) by case. The rule is simple and never varies.

**Prototypes use PascalCase.** The first letter is capitalized and each subsequent word is capitalized. Examples from the core library: `Object`, `List`, `Map`, `Sequence`, `Block`, `Coroutine`, `UnitTest`. When you clone a prototype to define a new type, keep the same convention:

```
Animal := Object clone
Dog := Animal clone
HttpRequest := Object clone
```

**Methods and variables use camelCase.** The first letter is lowercase; subsequent words are capitalized. Examples: `asString`, `appendSeq`, `removeFirst`, `slotNames`.

```
greetingFor := method(name, "hello, " .. name)
firstMatch := list select(isActive) first
```

**Constants defined on a proto also use camelCase** — Io does not have a separate SCREAMING_CASE convention. A constant is just a slot whose value you don't intend to change.

### Method name prefixes

The standard library uses a small vocabulary of prefixes to convey what a method does without reading its body. Follow these when adding methods to your own protos.

- **`as`** — returns a (possibly modified) copy in a different representation. The receiver is not mutated. Examples: `asString`, `asNumber`, `asUppercase`, `asList`, `asMutable`, `asJson`.
- **`to`** — converts toward a target, often destructively or with a target argument. Examples: `Sequence asUTF8`, conversions that write into something else.
- **`set`** — assigns a value and returns self, enabling chaining. Examples: `setName`, `setSize`, `setEncoding`.
- **`is`** — boolean query. Examples: `isNil`, `isEmpty`, `isKindOf`, `isActivatable`.
- **`has`** — boolean query about containment or possession. Examples: `hasSlot`, `hasProto`.
- **`with`** — constructor-style class method that returns a newly-configured clone. Example: `Date withNow`, `Range with(1, 10)`.
- **`for`** — returns something derived for a given key or context. Example: `slotSummaryFor`.

### In-place vs. copy

When a method has both a copying and a mutating form, the mutating form ends in `InPlace`. The copying form is the plain name.

```
"abc" asUppercase        // "ABC" — returns a new sequence
aMutableSeq uppercase     // modifies in place, returns self
```

The core `Sequence` proto follows this rigorously: `asUppercase` / `uppercase`, `asLowercase` / `lowercase`, `alignLeft` / `alignLeftInPlace`, etc. Methods that unconditionally mutate the receiver do not need the suffix — e.g. `List append`, `List removeFirst`.

### Private slots

A slot whose name begins with an underscore (`_`) is a hint that it is internal and should not be relied on from outside. Io does not enforce this; it is a reader-facing convention.

## Indentation and whitespace

**Use tabs for indentation** in both C and Io source files. One tab per level; do not mix tabs and spaces.

**No trailing whitespace** on any line.

**One blank line** between method definitions at the same nesting level. Two blank lines are reserved for separating top-level sections within a file.

**Spaces around binary operators** and after commas:

```
x := a + b * c
list := list(1, 2, 3)
```

**No space between a message and its argument list.** `foo(x)`, not `foo (x)`.

**Chain message sends with single spaces:**

```
people select(isActive) map(name) sort join(", ")
```

Long chains can break across lines; align the continuation with the receiver or indent one level:

```
result := people \
    select(isActive) \
    map(name) \
    sort \
    join(", ")
```

## Comments

Keep comments minimal. Well-named methods and slot names should carry most of the meaning. Write a comment when the *why* is non-obvious — a constraint, an invariant, or a workaround — not to restate what the code plainly does.

Io's reflection system recognizes two documentation forms that tools can extract:

**`//doc`** — a single-line doc comment attached to a slot. Placed immediately before the slot definition.

```
//doc List sum Returns the sum of the items.
sum := method(self reduce(+))
```

**`/*doc ... */`** — a multi-line doc comment for longer descriptions.

```
/*doc List join(optionalSeparator)
Returns a Sequence of the concatenated items with optionalSeparator
between each item, or simply the concatenation of the items if no
optionalSeparator is supplied.
*/
join := method(sep, ...)
```

**`//metadoc`** — metadata about a proto itself (one per proto, at the top of the file).

```
//metadoc List category Core
//metadoc List description A mutable, ordered collection of values.
```

Ordinary `//` and `/* */` comments are still available for implementation notes that aren't intended for doc extraction.

## File organization

Each proto lives in its own file named after the proto: `List.io`, `Sequence.io`, `Map.io`. The file defines and extends the proto using a `do(...)` block:

```
List do(
    sum := method(self reduce(+))
    average := method(self sum / self size)
    ...
)
```

Inside the `do` block, order slots roughly from fundamental to derived: state slots first, then core operations, then conveniences built on them. Group related methods together.

Bootstrap files that must load before other core protos end in `_bootstrap.io` (e.g. `List_bootstrap.io`, `Object_bootstrap.io`). They are listed explicitly at the top of the `IO_FILES` variable in the Makefile — load order is not alphabetical.

## Method patterns

### Cloning and initialization

A new instance is always a clone of an existing proto. If initialization is needed, override `init`:

```
Animal := Object clone do(
    name ::= nil
    legs ::= 4

    init := method(
        self sounds := list()
    )
)
```

`::=` declares a slot and generates a matching setter (`setName`, `setLegs`). Use it for public, settable state. Use `:=` when you want the slot without a generated setter.

### Chaining

Methods that modify the receiver should return `self` so they can be chained:

```
list append(1) append(2) append(3)
```

Methods that produce a derived value return that value — `asUppercase`, `map`, `select` do not return self.

### Fluent constructors

When a proto is typically created with several initial settings, provide a `with` class method:

```
Range := Object clone do(
    with := method(start, end,
        r := self clone
        r start := start
        r end := end
        r
    )
)

r := Range with(1, 10)
```

## Error handling

Raise exceptions for exceptional conditions; return `nil` for ordinary "not found" results.

```
// "not found" — return nil
list detect(isPrime)        // nil if none

// exceptional — raise
Exception raise("connection lost: " .. host)
```

Catch exceptions with `try`:

```
e := try(riskyOperation)
if(e, e showStack)
```

Subclass `Exception` for error categories you want callers to distinguish:

```
ParseError := Exception clone
```

## Testing

Tests live alongside the code they exercise, in files named `*Test.io`, and extend `UnitTest`:

```
ListTest := UnitTest clone do(
    testAppend := method(
        l := list(1, 2)
        l append(3)
        assertEquals(l, list(1, 2, 3))
    )

    testRemoveFirst := method(
        assertEquals(list(1, 2, 3) removeFirst, 1)
    )
)
```

Each test method begins with `test`. Use `assertEquals`, `assertTrue`, `assertRaisesException`, and the other assertions on `UnitTest`.

## Commit messages

Short, imperative first line describing *what changed and why*. Reference the issue or PR number if relevant.

```
Fix List removeFirst to return nil on empty list (#123)
```

Longer explanation goes in the body, separated from the subject by a blank line. Keep the subject under ~70 characters.

Prefer small, focused commits over mixed ones. Each commit should leave the tree in a buildable, testable state.
