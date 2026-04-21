---
heroImage: ../images/Objects.jpg
---

# Objects

Slots, prototypes, cloning, and differential inheritance.

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
