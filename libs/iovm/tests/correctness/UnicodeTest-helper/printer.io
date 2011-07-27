#!/usr/bin/env io

scriptPath := method(call message label pathComponent) call
Importer addSearchPath(Path with(scriptPath, ".."))

args := System args
if(args at(1) == "--print",
	UnicodeTest getSlot(args at(2)) print
	exit
)

if(args at(1) == "--arg",
	args at(2) asMutable replaceSeq("\\n", "\n") print
	exit
)
