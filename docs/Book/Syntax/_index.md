---
heroImage: ../images/Syntax.png
---

# Syntax

Io's minimal grammar: expressions, messages, and operators.

## Expressions

Io has no keywords or statements. Everything is an expression composed entirely of messages, each of which is a runtime accessible object. The informal BNF description:

```io
exp        ::= { message | terminator }
message    ::= symbol [arguments]
arguments  ::= "(" [exp [ { "," exp } ]] ")"
symbol     ::= identifier | number | string
terminator ::= "\n" | ";"
```


For performance reasons, String and Number literal messages have their results cached in their message objects.


## Messages

Message arguments are passed as expressions and evaluated by the receiver. Selective evaluation of arguments can be used to implement control flow. Examples:

```io
for(i, 1, 10, i println)
a := if(b == 0, c + 1, d)
```


In the above code, "for" and "if" are just normal messages, not special forms or keywords.

Likewise, dynamic evaluation can be used with enumeration without the need to wrap the expression in a block. Examples:

```io
people select(person, person age < 30)
names := people map(person, person name)
```


Methods like map and select will typically apply the expression directly to the values if only the expression is provided:

```io
people select(age < 30)
names := people map(name)
```


There is also some syntax sugar for operators (including assignment), which are handled by an Io macro executed on the expression after it is compiled into a message tree. Some sample source code:

```io
Account := Object clone
Account balance := 0
Account deposit := method(amount,
balance = balance + amount
)
```


```io
account := Account clone
account deposit(10.00)
account balance println
```


Like Self[2], Io's syntax does not distinguish between accessing a slot containing a method from one containing a variable.



## Operators

An operator is just a message whose name contains no alphanumeric characters (other than ";", "_", '"' or ".") or is one of the following words: or, and, return. Example:

```io
1 + 2
```


This just gets compiled into the normal message:

```io
1 +(2)
```


Which is the form you can use if you need to do grouping:

```io
1 +(2 * 4)
```


Standard operators follow C's precedence order, so:

```io
1 + 2 * 3 + 4
```


Is parsed as:

```io
1 +(2 *(3)) +(4)
```


User defined operators (that don't have a standard operator name) are performed left to right.


## Assignment

Io has three assignment operators:


These operators are compiled to normal messages whose methods can be overridden.


For example:


On Locals objects, updateSlot is overridden so it will update the slot in the object in which the method was activated if the slot is not found the locals. This is done so update assignments in methods don't require self to be an explicit target.



## Numbers

The following are examples of valid number formats:

```io
123
123.456
0.456
.456
123e-4
123e4
123.456e-7
123.456e2
```


Hex numbers are also supported (in any casing):

```io
0x0
0x0F
0XeE
```



## Strings

Strings can be defined surrounded by a single set of double quotes with escaped quotes (and other escape characters) within.

```io
s := "this is a \"test\".\nThis is only a test."
```


Or for strings with non-escaped characters and/or spanning many lines, triple quotes can be used.

```io
s := """this is a "test".
This is only a test."""
```


## Comments

Comments of the //, /**/ and # style are supported. Examples:

```io
a := b // add a comment to a line
```


```io
/* comment out a group
a := 1
b := 2
*/
```


The "#" style is useful for unix scripts:

```io
#!/usr/local/bin/io
```


That's it! You now know everything there is to know about Io's syntax. Control flow, objects, methods, exceptions are expressed with the syntax and semantics described above.
