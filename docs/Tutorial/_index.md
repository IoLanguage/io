# Tutorial

A short, example-driven tour of Io for folks who already know how to program. Each section is a quick REPL session.

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
