# Book

The original Io book, converted from the iBooks Author bundle.

# Introduction


## Overview

Io is a dynamic prototype-based programming language. The ideas in Io are mostly inspired by Smalltalk[1] (all values are objects), Self[2] (prototype-based), NewtonScript[3] (differential inheritance), Act1[4] (actors and futures for concurrency), Lisp[5] (code is a runtime inspectable / modifiable tree) and Lua[6] (small, embeddable).




## Perspective

The focus of programming language research for the last thirty years has been to combine the expressive power of high level languages like Smalltalk and the performance of low level language like C with little attention paid to advancing expressive power itself. The result has been a series of languages which are neither as fast as C or as expressive as Smalltalk. Io's purpose is to refocus attention on expressiveness by exploring higher level dynamic programming features with greater levels of runtime flexibility and simplified programming syntax and semantics.

In Io, all values are objects (of which, anything can change at runtime, including slots, methods and inheritance), all code is made up of expressions (which are runtime inspectable and modifiable) and all expressions are made up of dynamic message sends (including assignment and control structures). Execution contexts themselves are objects and activatable objects such as methods/blocks and functions are unified into blocks with assignable scope. Concurrency is made more easily manageable through actors and implemented using coroutines for scalability.




## Getting Started

Io distributions are available at:

```io
[http://iolanguage.com](http://iolanguage.com)
```



### Interactive Mode

If Io is installed, running:

```io
io
```


will open the Io interpreter prompt.

You can evaluate code by entering it directly. Example:

```io
Io> "Hello world!" println
==> Hello world!
```


Expressions are evaluated in the context of the Lobby:

```io
Io> print
[printout of lobby contents]
```


If you have a .iorc file in your home folder, it will be evaled before the interactive prompt starts.


#### Inspecting objects

You can get a list of the slots of an object like this:

```io
Io> someObject slotNames
```


To show them in sorted order:

```io
Io> someObject slotNames sort
```


For a nicely formatted description of an object, the slotSummary method is handy:

```io
Io> slotSummary
==>  Object_0x20c4e0:
Lobby = Object_0x20c4e0
Protos = Object_0x20bff0
exit = method(...)
forward = method(...)
```



Exploring further:

```io
Io> Protos
==>  Object_0x20bff0:
Addons = Object_0x20c6f0
Core = Object_0x20c4b0
```


```io
Io> Protos Addons
==>  Object_0x20c6f0:
ReadLine = Object_0x366a10
```


Only ReadLine is seen in the Addons since no other Addons have been loaded yet.

Inspecting a method will print a decompiled version of it:


```io
Io> Lobby getSlot("forward")
==> # io/Z_Importer.io:65
method(
Importer import(call)
)
```


#### doFile and doString

A script can be run from the interactive mode using the doFile method:

```io
doFile("scriptName.io")
```


The evaluation context of doFile is the receiver, which in this case would be the lobby. To evaluate the script in the context of some other object, simply send the doFile message to it:

```io
someObject doFile("scriptName.io")
```


The doString method can be used to evaluate a string:

```io
Io> doString("1+1")
==> 2
```


And to evaluate a string in the context of a particular object:

```io
someObject doString("1 + 1")
```


### Command Line Arguments

Example of printing out command line arguments:

```io
System args foreach(k, v, write("'", v, "'\n"))
```


### launchPath

The System "launchPath" slot is set to the location of the initial source file that is executed; when the interactive prompt is started (without specifying a source file to execute), the launchPath is the current working directory:

```io
System launchPath
```



# Syntax


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







# Objects


## Overview

Io's guiding design principle is simplicity and power through conceptual unification.


## Prototypes

In Io, everything is an object (including the locals storage of a block and the namespace itself) and all actions are messages (including assignment). Objects are composed of a list of key/value pairs called slots, and an internal list of objects from which it inherits called protos. A slot's key is a symbol (a unique immutable sequence) and its value can be any type of object.


### clone and init

New objects are made by cloning existing ones. A clone is an empty object that has the parent in its list of protos. A new instance's init slot will be activated which gives the object a chance to initialize itself. Like NewtonScript[3], slots in Io are create-on-write.

```io
me := Person clone
```


To add an instance variable or method, simply set it:

```io
myDog name := "rover"
myDog sit := method("I'm sitting\n" print)
```


When an object is cloned, its "init" slot will be called if it has one.


### Inheritance

When an object receives a message it looks for a matching slot, if not found, the lookup continues depth first recursively in its protos. Lookup loops are detected (at runtime) and avoided. If the matching slot contains an activatable object, such as a Block or CFunction, it is activated, if it contains any other type of value it returns the value. Io has no globals and the root object in the Io namespace is called the Lobby.

Since there are no classes, there's no difference between a subclass and an instance. Here's an example of creating the equivalent of a subclass:

```io
Io> Dog := Object clone
==> Object_0x4a7c0
```


The above code sets the Lobby slot "Dog" to a clone of the Object object; the protos list of this new object contains only a reference to Object, essentially indicating that a subclass of Object has been created.



Instance variables and methods are inherited from the objects referenced in the protos list. If a slot is set, it creates a new slot in our object instead of changing the protos:

```io
Io> Dog color := "red"
Io> Dog
==> Object_0x4a7c0:
color := "red"
```


#### Multiple Inheritance

You can add any number of protos to an object's protos list. When responding to a message, the lookup mechanism does a depth first search of the proto chain.



## Methods

A method is an anonymous function which, when called, creates an object to store its locals and sets the local's proto pointer and its self slot to the target of the message. The Object method method() can be used to create methods. Example:

```io
method((2 + 2) print)
```


An example of using a method in an object:

```io
Dog := Object clone
Dog bark := method("woof!" print)
```


The above code creates a new "subclass" of object named Dog and adds a bark slot containing a block that prints "woof!". Example of calling this method:

```io
Dog bark
```


The default return value of a block is the result of the last expression.


#### Arguments

Methods can also be defined to take arguments. Example:

```io
add := method(a, b, a + b)
```


The general form is:

```io
method(<arg name 0>, <arg name 1>, ..., <do message>)
```


### Blocks

A block is the same as a method except it is lexically scoped. That is, variable lookups continue in the context of where the block was created instead of the target of the message which activated the block. A block can be created using the Object method block(). Example of creating a block:

```io
b := block(a, a + b)
```


#### Blocks vs. Methods

This is sometimes a source of confusion so it's worth explaining in detail. Both methods and blocks create an object to hold their locals when they are called. The difference is what the "proto" and "self" slots of that locals object are set to. In a method, those slots are set to the target of the message. In a block, they're set to the locals object where the block was created. So a failed variable lookup in a block's locals continue in the locals where it was created. And a failed variable lookup in a method's locals continue in the object to which the message that activated it was sent.


#### call and self slots

When a locals object is created, its self slot is set (to the target of the message, in the case of a method, or to the creation context, in the case of a block) and its call slot is set to a Call object that can be used to access information about the block activation:


#### Variable Arguments

The "call message" slot in locals can be used to access the unevaluated argument messages. Example of implementing if() within Io:

```io
myif := method(
(call sender doMessage(call message argAt(0))) ifTrue(
call sender doMessage(call message argAt(1))) ifFalse(
call sender doMessage(call message argAt(2)))
)
```


```io
myif(foo == bar, write("true\n"), write("false\n"))
```


The doMessage() method evaluates the argument in the context of the receiver. A shorter way to express this is to use the evalArgAt() method on the call object:

```io
myif := method(
call evalArgAt(0) ifTrue(
call evalArgAt(1)) ifFalse(
call evalArgAt(2))
)
```


```io
myif(foo == bar, write("true\n"), write("false\n"))
```


#### Forward

If an object doesn't respond to a message, it will invoke its "forward" method if it has one. Here's an example of how to print the information related lookup that failed:

```io
MyObject forward := method(
write("sender = ", call sender, "\n")
write("message name = ", call message name, "\n")
args := call message argsEvaluatedIn(call sender)
args foreach(i, v, write("arg", i, " = ", v, "\n") )
)
```


#### Resend

Sends the current message to the receiver's protos with self as the context. Example:

```io
A := Object clone
A m := method(write("in A\n"))
B := A clone
B m := method(write("in B\n"); resend)
B m
```


will print:

```io
in B
in A
```


For sending other messages to the receiver's proto, super is used.


#### Super

Sometimes it's necessary to send a message directly to a proto. Example:

```io
Dog := Object clone
Dog bark := method(writeln("woof!"))
```


```io
fido := Dog clone
fido bark := method(
writeln("ruf!")
super(bark)
)
```


Both resend and super are implemented in Io.



## Introspection

Using the following methods you can introspect the entire Io namespace. There are also methods for modifying any and all of these attributes at runtime.


### slotNames

The slotNames method returns a list of the names of an object's slots:

```io
Io> Dog slotNames
==> list("bark")
```


### protos

The protos method returns a list of the objects which an object inherits from:

```io
Io> Dog protos
==> list("Object")
```


### getSlot

The "getSlot" method can be used to get the value of a block in a slot without activating it:

```io
myMethod := Dog getSlot("bark")
```


Above, we've set the locals object's "myMethod" slot to the bark method. It's important to remember that if you then want use the myMethod without activating it, you'll need to use the getSlot method:

```io
otherObject newMethod := getSlot("myMethod")
```


Here, the target of the getSlot method is the locals object.’


### code

The arguments and expressions of methods are open to introspection. A useful convenience method is "code", which returns a string representation of the source code of the method in a normalized form.

```io
Io> method(a, a * 2) code
==> "method(a, a *(2))"
```



# Control Flow


## Branching

### if, then, else

The if() method can be used in the form:

```io
if(<condition>, <do message>, <else do message>)
```


Example:

```io
if(a == 10, "a is 10" print)
```


The else argument is optional. The condition is considered false if the condition expression evaluates to false or nil, and true otherwise.

The result of the evaluated message is returned, so:

```io
if(y < 10, x := y, x := 0)
```


is the same as:

```io
x := if(y < 10, y, 0)
```



Conditions can also be used in this form:

```io
if(y < 10) then(x := y) else(x := 2)
```


elseif() is supported:

```io
if(y < 10) then(x := y) elseif(
y == 11) then(x := 0) else(x := 2)
```


### ifTrue, ifFalse

Also supported are Smalltalk style ifTrue, ifFalse, ifNil and ifNonNil methods:

```io
(y < 10) ifTrue(x := y) ifFalse(x := 2)
```


Notice that the condition expression must have parenthesis surrounding it.


#### loop

The loop method can be used for "infinite" loops:

```io
loop("foo" println)
```


### repeat

The Number repeat method can be used to repeat a loop a given number of times.

```io
3 repeat("foo" print)
==> foofoofoo
```


#### while

Arguments:

```io
while(<condition>, <do message>)
```


Example:

```io
a := 1
while(a < 10,
a print
a = a + 1
)
```


### for

Arguments:

```io
for(<counter>, <start>, <end>,
<optional step>, <do message>)
```


The start and end messages are only evaluated once, when the loop starts. Example:

```io
for(a, 0, 10,
a println
)
```


Example with a step:

```io
for(x, 0, 10, 3, x println)
```


Which would print:

```io
0
3
6
9
```


To reverse the order of the loop, add a negative step:

```io
for(a, 10, 0, -1, a println)
```


Note: the first value will be the first value of the loop variable and the last will be the last value on the final pass through the loop. So a loop of 1 to 10 will loop 10 times and a loop of 0 to 10 will loop 11 times.


### break, continue

loop, repeat, while and for support the break and continue methods. Example:

```io
for(i, 1, 10,
if(i == 3, continue)
if(i == 7, break)
i print
)
```


Outputs:

```io
12456
```


### return

Any part of a block can return immediately using the return method. Example:

```io
Io> test := method(123 print;
return "abc"; 456 print)
Io> test
123
==> abc
```


Internally, break, continue and return all work by setting a IoState internal variable called "stopStatus" which is monitored by the loop and message evaluation code.



## Comparison

### true, false and nil

There are singletons for true, false and nil. nil is typically used to indicate an unset or missing value.


### Comparison Methods

The comparison methods:

```io
==, !=, >=, <=, >, <
```


return either the true or false. The compare() method is used to implement the comparison methods and returns -1, 0 or 1 which mean less-than, equal-to or greater-than, respectively.




## Loops

### loop

The loop method can be used for "infinite" loops:

```io
loop("foo" println)
```


### repeat

The Number repeat method can be used to repeat a loop a given number of times.

```io
3 repeat("foo" print)
==> foofoofoo
```


### while

Arguments:

```io
while(<condition>, <do message>)
```


Example:

```io
a := 1
while(a < 10,
a print
a = a + 1
)
```


### for

Arguments:

```io
for(<counter>, <start>, <end>,
<optional step>, <do message>)
```


The start and end messages are only evaluated once, when the loop starts. Example:

```io
for(a, 0, 10,
a println
)
```


Example with a step:

```io
for(x, 0, 10, 3, x println)
```


Which would print:

```io
0
3
6
9
```


To reverse the order of the loop, add a negative step:

```io
for(a, 10, 0, -1, a println)
```


Note: the first value will be the first value of the loop variable and the last will be the last value on the final pass through the loop. So a loop of 1 to 10 will loop 10 times and a loop of 0 to 10 will loop 11 times.


### break, continue

loop, repeat, while and for support the break and continue methods. Example:

```io
for(i, 1, 10,
if(i == 3, continue)
if(i == 7, break)
i print
)
```


Output:

```io
12456
```


#### return

Any part of a block can return immediately using the return method. Example:

```io
Io> test := method(123 print; return "abc"; 456 print)
Io> test
123
==> abc
```


Internally, break, continue and return all work by setting a IoState internal variable called "stopStatus" which is monitored by the loop and message evaluation code.



## Exceptions

### Raise

An exception can be raised by calling raise() on an exception proto.

```io
Exception raise("generic foo exception")
```


### Try and Catch

To catch an exception, the try() method of the Object proto is used. try() will catch any exceptions that occur within it and return the caught exception or nil if no exception is caught.

```io
e := try(<doMessage>)
```


To catch a particular exception, the Exception catch() method can be used. Example:

```io
e := try(
// ...
)
```


```io
e catch(Exception,
writeln(e coroutine backtraceString)
)
```


The first argument to catch indicates which types of exceptions will be caught. catch() returns the exception if it doesn't match and nil if it does.


#### Pass

To re-raise an exception caught by try(), use the pass method. This is useful to pass the exception up to the next outer exception handler, usually after all catches failed to match the type of the current exception:

```io
e := try(
// ...
)
```


```io
e catch(Error,
// ...
) catch(Exception,
// ...
) pass
```


### Custom Exceptions

Custom exception types can be implemented by simply cloning an existing Exception type:

```io
MyErrorType := Error clone
```





# Concurrency


## Coroutines

Io uses coroutines (user level cooperative threads), instead of preemptive OS level threads to implement concurrency. This avoids the substantial costs (memory, system calls, locking, caching issues, etc) associated with native threads and allows Io to support a very high level of concurrency with thousands of active threads.


### Scheduler

The Scheduler object is responsible for resuming coroutines that are yielding. The current scheduling system uses a simple first-in-first-out policy with no priorities.





## Actors

An actor is an object with its own thread (in our case, its own coroutine) which it uses to process its queue of asynchronous messages. Any object in Io can be sent an asynchronous message by placing using the asyncSend() or futureSend() messages. Examples:

Synchronous:

```io
result := self foo
```


Asynchronous, immediately returns a Future:


```io
futureResult := self futureSend(foo)
```


Asynchronous, immediately returns nil:


```io
self asyncSend(foo)
```


When an object receives an asynchronous message it puts the message in its queue and, if it doesn't already have one, starts a coroutine to process the messages in its queue. Queued messages are processed sequentially in a first-in-first-out order. Control can be yielded to other coroutines by calling "yield".

Example:

```io
obj1 := Object clone
obj1 test := method(for(n, 1, 3, n print; yield))
obj2 := obj1 clone
obj1 asyncSend(test); obj2 asyncSend(test)
while(Scheduler yieldingCoros size > 1, yield)
```


This would print "112233".

Here's a more real world example:

```io
HttpServer handleRequest := method(aSocket,
HttpRequestHandler clone asyncSend(
handleRequest(aSocket)
)
)
```



## Futures

Io's futures are transparent. That is, when the result is ready, they become the result. If a message is sent to a future (besides the two methods it implements), it waits until it turns into the result before processing the message. Transparent futures are powerful because they allow programs to minimize blocking while also freeing the programmer from managing the fine details of synchronization.


### Auto Deadlock Detection

An advantage of using futures is that when a future requires a wait, it will check to see if pausing to wait for the result would cause a deadlock and if so, avoid the deadlock and raise an exception. It performs this check by traversing the list of connected futures.


### Futures and the Command Line Interface

The command line will attempt to print the result of expressions evaluated in it, so if the result is a Future, it will attempt to print it and this will wait on the result of Future. Example:

```io
Io> q := method(wait(1))
Io> futureSend(q)
[1-second delay]
==> nil
```


To avoid this, just make sure the Future isn't the result. Example:

```io
Io> futureSend(q); nil
[no delay]
==> nil
```


### Yield

An object will automatically yield between processing each of its asynchronous messages. The yield method only needs to be called if a yield is required during an asynchronous message execution.


### Pause and Resume

It's also possible to pause and resume an object. See the concurrency methods of the Object primitive for details and related methods.




# Primitives


## Primitives

Primitives are objects built into Io whose methods are typically implemented in C and store some hidden data in their instances. For example, the Number primitive has a double precision floating point number as its hidden data and its methods that do arithmetic operations are C functions. All Io primitives inherit from the Object prototype and are mutable. That is, their methods can be changed. The reference docs contain more info on primitives.

This document is not meant as a reference manual, but an overview of the base primitives and bindings is provided here to give the user a jump start and a feel for what is available and where to look in the reference documentation for further details.


### Object

#### The ? Operator

Sometimes it's desirable to conditionally call a method only if it exists (to avoid raising an exception). Example:

```io
if(obj getSlot("foo"), obj foo)
```


Putting a "?" before a message has the same effect:

```io
obj ?foo
```


### List

A List is an array of references and supports all the standard array manipulation and enumeration methods. Examples:

Create an empty list:

```io
a := List clone
```


Create a list of arbitrary objects using the list() method:

```io
a := list(33, "a")
```


Append an item:

```io
a append("b")
==> list(33, "a", "b")
```


Get the list size:

```io
a size
==> 3
```


Get the item at a given index (List indexes begin at zero):

```io
a at(1)
==> "a"
```


Note: List indexes begin at zero and nil is returned if the accessed index doesn't exist.


Set the item at a given index:

```io
a atPut(2, "foo")
==> list(33, "a", "foo", "b")
```


```io
a atPut(6, "Fred")
==> Exception: index out of bounds
```


Remove an item at a given index:

```io
a remove("foo")
==> list(33, "a", "b")
```


Inserting an item at a given index:

```io
a atInsert(2, "foo")
==> list(33, "a", "foo", "56")
```


#### foreach

The foreach, map and select methods can be used in three forms:

```io
Io> a := list(65, 21, 122)
```


In the first form, the first argument is used as an index variable, the second as a value variable and the 3rd as the expression to evaluate for each value.

```io
Io> a foreach(i, v, write(i, ":", v, ", "))
==> 0:65, 1:21, 2:122,
```


The second form removes the index argument:

```io
Io> a foreach(v, v println)
==> 65
21
122
```


The third form removes the value argument and simply sends the expression as a message to each value:

```io
Io> a foreach(println)
==> 65
21
122
```


#### map and select

Io's map and select (known as filter in some other languages) methods allow arbitrary expressions as the map/select predicates.

```io
Io> numbers := list(1, 2, 3, 4, 5, 6)
```


```io
Io> numbers select(isOdd)
==> list(1, 3, 5)
```


```io
Io> numbers select(x, x isOdd)
==> list(1, 3, 5)
```


```io
Io> numbers select(i, x, x isOdd)
==> list(1, 3, 5)
```


```io
Io> numbers map(x, x*2)
==> list(2, 4, 6, 8, 10, 12)
```


```io
Io> numbers map(i, x, x+i)
==> list(1, 3, 5, 7, 9, 11)
```


```io
Io> numbers map(*3)
==> list(3, 6, 9, 12, 15, 18)
```


The map and select methods return new lists. To do the same operations in-place, you can use selectInPlace() and mapInPlace() methods.

### Sequence

In Io, an immutable Sequence is called a Symbol and a mutable Sequence is the equivalent of a Buffer or String. Literal strings(ones that appear in source code surrounded by quotes) are Symbols. Mutable operations cannot be performed on Symbols, but one can make mutable copy of a Symbol calling its asMutable method and then perform the mutation operations on the copy. Common string operations Getting the length of a string:

```io
"abc" size
==> 3
```


Checking if a string contains a substring:

```io
"apples" containsSeq("ppl")
==> true
```


Getting the character (byte) at position N:

```io
"Kavi" at(1)
==> 97
```



Slicing:

```io
"Kirikuro" slice(0, 2)
==> "Ki"
```


```io
"Kirikuro" slice(-2)  # NOT: slice(-2, 0)!
==> "ro"
```


```io
Io> "Kirikuro" slice(0, -2)
# "Kiriku"
```


Stripping whitespace:

```io
"  abc  " asMutable strip
==> "abc"
```


```io
"  abc  " asMutable lstrip
==> "abc  "
```


```io
"  abc  " asMutable rstrip
==> "  abc"
```


Converting to upper/lowercase:

```io
"Kavi" asUppercase
==> "KAVI"
"Kavi" asLowercase
==> "kavi"
```


Splitting a string:

```io
"the quick brown fox" split
==> list("the", "quick", "brown", "fox")
```


Splitting by others character is possible as well.

```io
"a few good men" split("e")
==> list("a f", "w good m", "n")
```


Converting to number:

```io
"13" asNumber
==> 13
"a13" asNumber
==> nil
```


String interpolation:

```io
name := "Fred"
==> Fred
"My name is #{name}" interpolate
==> My name is Fred
```


Interpolate will eval anything with #{} as Io code in the local context. The code may include loops or anything else but needs to return an object that responds to asString.


### Ranges

A range is a container containing a start and an end point, and instructions on how to get from the start, to the end. Using Ranges is often convenient when creating large lists of sequential data as they can be easily converted to lists, or as a replacement for the for() method.

#### The Range protocol

Each object that can be used in Ranges needs to implement a "nextInSequence" method which takes a single optional argument (the number of items to skip in the sequence of objects), and return the next item after that skip value. The default skip value is 1. The skip value of 0 is undefined. An example:

```io
Number nextInSequence := method(skipVal,
if(skipVal isNil, skipVal = 1)
self + skipVal
)
```


With this method on Number (it's already there in the standard libraries), you can then use Numbers in Ranges, as demonstrated below:

```io
1 to(5) foreach(v, v println)
```


The above will print 1 through 5, each on its own line.


### File

The methods openForAppending, openForReading, or openForUpdating are used for opening files. To erase an existing file before opening a new open, the remove method can be used. Example:

```io
f := File with("foo.txt)
f remove
f openForUpdating
f write("hello world!")
f close
```


### Directory

Creating a directory object:

```io
dir := Directory with("/Users/steve/")
```


Get a list of file objects for all the files in a directory:

```io
files := dir files
==> list(File_0x820c40, File_0x820c40, ...)
```


Get a list of both the file and directory objects in a directory:

```io
items := Directory items
==> list(Directory_0x8446b0, File_0x820c40, ...)
```


```io
items at(4) name
==> DarkSide-0.0.1 # a directory name
```


Setting a Directory object to a certain directory and using it:

```io
root := Directory clone setPath("c:/")
==> Directory_0x8637b8
```


```io
root fileNames
==> list("AUTOEXEC.BAT", "boot.ini", "CONFIG.SYS", ...)
```


Testing for existence:

```io
Directory clone setPath("q:/") exists
==> false
```


Getthing the current working directory:

```io
Directory currentWorkingDirectory
==> "/cygdrive/c/lang/IoFull-Cygwin-2006-04-20"
```


### Date

Creating a new date instance:

```io
d := Date clone
```


Setting it to the current date/time:

```io
d now
```


Getting the date/time as a number, in seconds:

```io
Date now asNumber
==> 1147198509.417114
```


Getting individual parts of a Date object:

```io
d := Date now
==> 2006-05-09 21:53:03 EST
d
==> 2006-05-09 21:53:03 EST
d year
==> 2006
d month
==> 5
d day
==> 9
d hour
==> 21
d minute
==> 53
d second
==> 3.747125
```


Find how long it takes to execute some code:


```io
Date cpuSecondsToRun(100000 repeat(1+1))
==> 0.02
```


### Networking

All of Io's networking is done with asynchronous sockets underneath, but operations like reading and writing to a socket appear to be synchronous since the calling coroutine is unscheduled until the socket has completed the operation, or a timeout occurs. Note that you'll need to first reference the associated addon in order to cause it to load before using its objects. In these examples, you'll have to reference "Socket" to get the Socket addon to load first.


Creating a URL object:

```io
url := URL with("[http://example.com](http://example.com)/")
```


Fetching an URL:

```io
data := url fetch
```


Streaming a URL to a file:

```io
url streamTo(File with("out.txt"))
```


A simple whois client:

```io
whois := method(host,
socket := Socket clone \
setHostName("rs.internic.net") setPort(43)
socket connect streamWrite(host, "\n")
while(socket streamReadNextChunk, nil)
return socket readBuffer
)
```


A minimal web server:

```io
WebRequest := Object clone do(
handleSocket := method(aSocket,
aSocket streamReadNextChunk
request := aSocket readBuffer \
betweenSeq("GET ", " HTTP")
f := File with(request)
if(f exists,
f streamTo(aSocket)
,
aSocket streamWrite("not found")
)
aSocket close
)
)
```


```io
WebServer := Server clone do(
setPort(8000)
handleSocket := method(aSocket,
WebRequest clone asyncSend(handleSocket(aSocket))
)
)
```


```io
WebServer start
```


### XML

Using the XML parser to find the links in a web page:

```io
SGML // reference this to load the SGML addon
xml := URL with("[http://www.yahoo.com](http://www.yahoo.com)/") fetch asXML
links := xml elementsWithName("a") map(attributes at("href"))
```


### Vector

Io's Vectors are built on its Sequence primitive and are defined as:

```io
Vector := Sequence clone setItemType("float32")
```


The Sequence primitive supports SIMD acceleration on a number of float32 operations. Currently these include add, subtract, multiple and divide but in the future can be extended to support most math, logic and string manipulation related operations.

Here's a small example:

```io
iters := 1000
size := 1024
ops := iters * size
```


```io
v1 := Vector clone setSize(size) rangeFill
v2 := Vector clone setSize(size) rangeFill
```


```io
dt := Date secondsToRun(
iters repeat(v1 *= v2)
)
```


```io
writeln((ops/(dt*1000000000)) asString(1, 3), " GFLOPS")
```


Which when run on 2Ghz Mac Laptop, outputs:

```io
1.255 GFLOPS
```


A similar bit of C code (without SIMD acceleration) outputs:

```io
0.479 GFLOPS
```


So for this example, Io is about three times faster than plain C.




## Unicode

### Sequences

In Io, symbols, strings, and vectors are unified into a single Sequence prototype which is an array of any available hardware data type such as:

```io
uint8, uint16, uint32, uint64
int8, int16, int32, int64
float32, float64
```


### Encodings

Also, a Sequence has a encoding attribute, which can be:

```io
number, ascii, ucs2, ucs4, utf8
```


UCS-2 and UCS-4 are the fixed character width versions of UTF-16 and UTF-32, respectively. A String is just a Sequence with a text encoding, a Symbol is an immutable String and a Vector is a Sequence with a number encoding.

UTF encodings are assumed to be big endian.

Except for input and output, all strings should be kept in a fixed character width encoding. This design allows for a simpler implementation, code sharing between vector and string ops, fast index-based access, and SIMD acceleration of Sequence operations. All Sequence methods will do automatic type conversions as needed.


### Source Code

Io source files are assumed to be in UTF8 (of which ASCII is a subset). When a source file is read, its symbols and strings are stored in Sequences in their minimal fixed character width encoding. Examples:

```io
Io> "hello" encoding
==> ascii
```


```io
Io> "π" encoding
==> ucs2
```


```io
Io> "∞" encoding
==> ucs2
```


We can also inspect the internal representation:


```io
Io> "π" itemType
==> uint16
```


```io
Io> "π" itemSize
==> 2
```


### Conversion

The Sequence object has a number of conversion methods:

```io
asUTF8
asUCS2
asUCS4
```







## Importing

The Importer proto implements Io's built-in auto importer feature. If you put each of your proto's in their own file, and give the file the same name with and ".io" extension, the Importer will automatically import that file when the proto is first referenced. The Importer's default search path is the current working directory, but can add search paths using its addSearchPath() method.



# Appendix


## Grammar

#### messages

expression ::= { message | sctpad }

message ::= [wcpad] symbol [scpad] [arguments]

arguments ::= Open [argument [ { Comma argument } ]] Close

argument ::= [wcpad] expression [wcpad]



#### symbols

symbol ::= Identifier | number | Opereator | quote

Identifier ::= { letter | digit | "_" }

Operator ::= { ":" | "." | "'" | "~" | "!" | "@" | "$" |

"%" | "^" | "&" | "*" | "-" | "+" | "/" | "=" | "{" | "}" |

"[" | "]" | "|" | "\" | "<" | ">" | "?" }



#### quotes

quote ::= MonoQuote | TriQuote

MonoQuote ::= """ [ "\"" | not(""")] """

TriQuote ::= """"" [ not(""""")] """""



#### spans

Terminator ::= { [separator] ";" | "\n" | "\r" [separator] }

separator ::= { " " | "\f" | "\t" | "\v" }

whitespace ::= { " " | "\f" | "\r" | "\t" | "\v" | "\n" }

sctpad ::= { separator | Comment | Terminator }

scpad ::= { separator | Comment }

wcpad ::= { whitespace | Comment }


#### comments

Comment ::= slashStarComment | slashSlashComment | poundComment

slashStarComment ::= "/*" [not("*/")] "*/"

slashSlashComment ::= "//" [not("\n")] "\n"

poundComment ::= "#" [not("\n")] "\n"



#### numbers

number ::= HexNumber | Decimal

HexNumber ::= "0" anyCase("x") { [ digit | hexLetter ] }

hexLetter ::= "a" | "b" | "c" | "d" | "e" | "f"

Decimal ::= digits | "." digits |

digits "." digits ["e" [-] digits]



#### characters

Comma ::= ","

Open ::= "(" | "[" | "{"

Close ::= ")" | "]" | "}"

letter ::= "a" ... "z" | "A" ... "Z"

digit ::= "0" ... "9"

digits ::= { digit }



Uppercase words designate elements the lexer treats as tokens.




## References


#### 1

Goldberg, A et al.

Smalltalk-80: The Language and Its Implementation

Addison-Wesley, 1983


#### 2

Ungar, D and Smith,

RB. Self: The Power of Simplicity

OOPSLA, 1987


#### 3

Smith, W.

Class-based NewtonScript Programming

PIE Developers magazine, Jan 1994






#### 4

Lieberman

H. Concurrent Object-Oriented Programming in Act 1

MIT AI Lab, 1987


#### 5

McCarthy, J et al.

LISP I programmer's manual

MIT Press, 1960


#### 6

Ierusalimschy, R, et al.

Lua: an extensible extension language




