#!/usr/bin/env io
isDebugging := false

objs := List clone

a := Object clone
a init := method(
  objs append(self)
  if (isDebugging, writeln("a ", self asSimpleString, " init"))
)

b := a clone
b init := method(
  super(init)
  objs append(self)
  if (isDebugging, writeln("b ", self asSimpleString, " init"))
)

c := b clone
c init := method(
  super(init)
  objs append(self)
  if (isDebugging, writeln("c ", self asSimpleString, " init"))
)

if (isDebugging, writeln("------------------"))

objs empty
objs println
t := c clone

objs append(t)

objs foreach(obj,
  if (obj != objs first, 
    writeln("ERROR [not all selfs are the same]")
    exit
  )
)
writeln("OK")

if (isDebugging, write("t := ", t uniqueId, " init\n"))
