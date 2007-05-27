#!/usr/bin/env io

// evaluating a string

writeln("1 + 2 ==> ", doString("1 + 2"))

simpleDecompile := method(
	// test of decompiling a method
	Dog := Object clone
	Dog bark := method("woof!" print)

	writeln("simple decompiled method: ", Dog getSlot("bark") code)
)

fancyDecompile := method(
	writeln("fancy decompile: ", Lobby getSlot("simpleDecompile") asString)
)

simpleDecompile
fancyDecompile
