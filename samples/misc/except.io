#!/usr/bin/env io
e := try(foo)
e catch(
	writeln
	writeln("caught: ", e error)
	writeln("standard reporting:")
	e showStack
)
writeln("done")
