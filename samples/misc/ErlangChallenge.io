#!/usr/bin/env io

//Collector debugOn

Test := Object clone do(
	next ::= nil
	id ::= nil
	ping := method(
		//writeln("ping ", id)
		if(next, next @@ping)
		yield
	)
)

max := 10000

t := Test clone

setup := method(
	for(i, 1, max, 
		t := Test clone setId(i) setNext(t)
		t @@id 
		yield
	)
)

writeln(max, " coros")	
writeln(Date secondsToRun(setup), " secs to setup")	
writeln(Date secondsToRun(t ping; yield), " secs to ping")	
