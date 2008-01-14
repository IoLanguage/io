#!/usr/bin/env io

Importer addSearchPath(Path with(method(call message label pathComponent) call, ".."))

args := System
if(args at(1) == "--print",
	UnicodeTest getSlot(args at(2)) print
	exit
)

if(args at(1) == "--arg",
	args at(2) print
	exit
)
