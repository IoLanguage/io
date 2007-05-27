#!/usr/bin/env io
// pythonish list and map syntax in Io

Object squareBrackets := Object getSlot("list")

a := [1, 2, 3] 
a println

curlyBrackets := method(
	map := Map clone
	call message arguments foreach(arg, arg setName("atPut"); map doMessage(arg))
	map
)

{a := 1, b := 2} foreach(k, v, writeln(k, ": ", v))
