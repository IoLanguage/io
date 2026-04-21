# Tutorial

A short, example-driven tour of Io for folks who already know how to program. Each section is a quick REPL session.

## Getting Started

Run the REPL and exit with `exit` or Ctrl-D:

```
$ io
Io 2025
Io> "hello" println
hello
==> hello

Io> exit
```

Run a script file:

```
$ io script.io
```

Evaluate a single expression:

```
$ io -e '"hello" println'
```

## Math

```
Io> 1+1
==> 2

Io> 2 sin
==> 0.909297

Io> 2 sqrt
==> 1.414214
```

## Variables

```
Io> a := 1
==> 1

Io> a
==> 1

Io> b := 2 * 3
==> 6

Io> a + b
==> 7
```

## Conditions

```
Io> a := 2

Io> if(a == 1) then(writeln("a is one")) else(writeln("a is not one"))
a is not one

Io> if(a == 1, writeln("a is one"), writeln("a is not one"))
a is not one
```

## Lists

```
Io> d := List clone append(30, 10, 5, 20)
==> list(30, 10, 5, 20)

Io> d size
==> 4

Io> d print
==> list(30, 10, 5, 20)

Io> d := d sort
==> list(5, 10, 20, 30)

Io> d first
==> 5

Io> d last
==> 30

Io> d at(2)
==> 20

Io> d remove(30)
==> list(5, 10, 20)

Io> d atPut(1, 123)
==> list(5, 123, 20)

Io> list(30, 10, 5, 20) select(>10)
==> list(30, 20)

Io> list(30, 10, 5, 20) detect(>10)
==> 30

Io> list(30, 10, 5, 20) map(*2)
==> list(60, 20, 10, 40)

Io> list(30, 10, 5, 20) map(v, v*2)
==> list(60, 20, 10, 40)
```

## Loops

```
Io> for(i, 1, 10, write(i, " "))
1 2 3 4 5 6 7 8 9 10

Io> d foreach(i, v, writeln(i, ": ", v))
0: 5
1: 123
2: 20

Io> list("abc", "def", "ghi") foreach(println)
abc
def
ghi
```

## Strings

```
Io> a := "foo"
==> "foo"

Io> b := "bar"
==> "bar"

Io> c := a .. b
==> "foobar"

Io> c at(0)
==> 102

Io> c at(0) asCharacter
==> "f"

Io> s := "this is a test"
==> "this is a test"

Io> words := s split(" ", "\t") print
"this", "is", "a", "test"

Io> s findSeq("is")
==> 2

Io> s findSeq("test")
==> 10

Io> s exSlice(10)
==> "test"

Io> s exSlice(2, 10)
==> "is is a "
```

## Maps

```
Io> m := Map clone
==> Map_0x...

Io> m atPut("first", "Alice")
Io> m atPut("age", 30)

Io> m at("first")
==> Alice

Io> m keys
==> list("first", "age")

Io> m size
==> 2

Io> m foreach(k, v, writeln(k, " = ", v))
first = Alice
age = 30
```

## Objects and Prototypes

Every object is cloned from another. There are no classes — only prototypes.

```
Io> Dog := Object clone
==> Dog_0x...

Io> Dog name := "Rex"
==> "Rex"

Io> Dog bark := method("woof!" println)
==> method(...)

Io> Dog bark
woof!
==> "woof!"
```

Clone an instance. Slots set on the clone override slots inherited from the prototype.

```
Io> fido := Dog clone
==> Dog_0x...

Io> fido name
==> "Rex"

Io> fido name := "Fido"
Io> fido name
==> "Fido"

Io> Dog name
==> "Rex"
```

## Methods

`method(args..., body)` creates a callable with its own local scope.

```
Io> square := method(x, x * x)
Io> square(5)
==> 25

Io> Dog greet := method(other, "Hi, " .. other name .. "!" println)
Io> fido greet(Dog clone setName("Spot"))
Hi, Spot!
```

Methods see the receiver as `self`. Assignment without a prefix writes into the receiver's slots.

```
Io> Counter := Object clone do(
    n := 0
    bump := method(n = n + 1)
)

Io> c := Counter clone
Io> c bump; c bump; c bump
Io> c n
==> 3
```

## Blocks

`block` is like `method`, but lexically scoped — it closes over the context where it was defined, not the receiver of the call.

```
Io> makeAdder := method(x, block(y, x + y))
Io> add3 := makeAdder(3)
Io> add3 call(4)
==> 7
```

## Inheritance

Clone to inherit. Override slots in the clone. `proto` points back to the parent.

```
Io> Animal := Object clone do(
    sound := "..."
    speak := method(sound println)
)

Io> Cat := Animal clone do(sound := "meow")
Io> Cat speak
meow

Io> Cat proto == Animal
==> true
```

Multiple inheritance: append protos to the `protos` list.

```
Io> Swimmer := Object clone do(swim := method("swimming" println))
Io> Flyer   := Object clone do(fly  := method("flying"  println))
Io> Duck    := Object clone
Io> Duck appendProto(Swimmer); Duck appendProto(Flyer)
Io> Duck swim
swimming
Io> Duck fly
flying
```

## Introspection

Every object knows its slots and protos at runtime.

```
Io> fido slotNames
==> list("name")

Io> Dog slotNames
==> list("name", "bark", "greet")

Io> fido getSlot("name")
==> "Fido"

Io> fido proto == Dog
==> true

Io> fido hasSlot("bark")
==> true
```

## Exceptions

```
Io> try(
    Exception raise("something broke")
) catch(Exception, e,
    writeln("caught: ", e error)
)
caught: something broke
```

A method can also `return` early or raise with `Exception raise`.

## File I/O

```
Io> f := File with("hello.txt")
Io> f openForUpdating
Io> f write("hello, world\n")
Io> f close

Io> File with("hello.txt") contents
==> "hello, world\n"
```

Iterate lines:

```
Io> File with("hello.txt") openForReading foreachLine(line, line println)
```

## Coroutines

Coroutines are cooperative, scheduled in-process. `yield` hands control back to the scheduler.

```
Io> task := method(n, for(i, 1, n, writeln("tick ", i); yield))

Io> task(3) @@; task(3) @@
Io> Scheduler waitForCorosToComplete
tick 1
tick 1
tick 2
tick 2
tick 3
tick 3
```

## Actors and Futures

Send a message asynchronously with `@` (returns a future) or `@@` (no return).

```
Io> slow := Object clone
Io> slow compute := method(x, System sleep(1); x * x)

Io> f := slow @compute(5)
Io> "doing other work..." println
Io> f                          // blocks until ready
==> 25
```

Any object can act as an actor — just send it async messages.

## Next Steps

- Read the [Book](../Book/index.html) for a deeper walkthrough of the language.
- Skim the [Reference](../Reference/index.html) for a complete list of built-in objects and methods.
- Browse the [Guide](../Guide/index.html) for idioms and conventions.
