#!/usr/bin/env io
weakLink := WeakLink clone setLink(Object clone)

writeln("WeakLink to ", weakLink link asSimpleString)

writeln("Collecting")
Collector collect

if(weakLink link == nil,
	writeln("Success: Object gone")
,
	writeln("Error: Object not gone")
)
