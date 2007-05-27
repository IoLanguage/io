#!/usr/bin/env io


a := "foobar"


writeln("test1: ",
	if(a == "foobar", "OK", "FAILED")
)

writeln("test2: ",
	if(a == "foo", "FAILED", "OK")
)

r := "FAILED"
if( a == "foobar" ) then(
	r = "OK"
) elseif( a == "C" ) then(
	r = "FAILED"
) else (
	r = "FAILED"
)
writeln("test3: ", r)


r := "FAILED"
if( a == "A" ) then (
	r = "FAILED"
) elseif( a == "foobar" ) then (
	r = "OK"
) else (
	r = "FAILED"
)
writeln("test4: ", r)

r := "FAILED"
if( a == "foobar" ) then (
	r = "OK"
) elseif( a == "C" ) then (
	r = "FAILED"
) else (
	r = "FAILED"
)
writeln("test5: ", r)
