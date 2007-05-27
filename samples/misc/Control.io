#!/usr/bin/env io
// example of implementing control structures

myif := method(condition,
	index := if(condition, 1, 2)
	call evalArgAt(index)
)

myif(19 > 5,
	writeln("19 > 5. Correct.")
,
	writeln("19 > 5. Incorrect.")
)

