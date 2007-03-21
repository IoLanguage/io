//debugParser := 1

debugOn := nil

objs := List clone

a := Object clone
a init := method(
  objs append(self)
  if (debugOn, write("a 0x", self uniqueId asString toBase(16), " init\n"))
)

b := a clone
b init := method(
  super(init)
  objs append(self)
  if (debugOn, write("b 0x", self uniqueId asString toBase(16), " init\n"))
)

c := b clone
c init := method(
  super(init)
  objs append(self)
  if (debugOn, write("c 0x", self uniqueId asString toBase(16), " init\n"))
)

if (debugOn, "------------------\n" print)

objs empty
objs print
t := c clone

objs append(t)

objs foreach(o, 
  if (o != objs first, 
    write("ERROR [not all selfs are the same]\n")
    exit
  )
)
write("OK\n")

if (debugOn, write("t := ", t uniqueId, " init\n"))
