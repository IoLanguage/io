---
heroImage: ../images/Introduction.jpg
---

# Introduction

The origins, philosophy, and influences behind Io.

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
