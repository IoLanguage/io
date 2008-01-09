#!/usr/bin/env io

/* The Computer Language Shootout
   http://shootout.alioth.debian.org
   Contributed by Gavin Harrison */

times := System args at(1) asNumber

Coroutine setStackSize(30000)

sum := 0

Thread := Object clone do(
	foo := method(n, 
		if(next, next @@foo(n + 1), Lobby sum = sum + n + 1)
	)
)

last := nil

500 repeat(
	t := Thread clone
	t next := last
	last = t
)

first := t
System args at(1) asNumber repeat(first @@foo(0))

while(Coroutine yieldingCoros size > 0, yield)

writeln(sum)

/*
times := System args at(1) asNumber

sum := 0

Thread := Coroutine clone do(
	init := method(
		self coro := Coroutine clone setStackSize(15000)
		coro setRunTarget(self) 
		coro setRunLocals(self)
		coro setRunMessage(message(process))
		coro run
	)
	
	receive := method(n,
		self n := n
		coro resume
	)
	
	process := method(
		mainCoro resume
		loop(
			if(next, next n := n + 1; next coro resume, Lobby sum = sum + n + 1; mainCoro resume)
		)
	)
)

last := nil

mainCoro := Coroutine currentCoroutine

500 repeat(
	t := Thread clone
	t next := last
	last = t
)

//Collector setDebug(true)

first := t
System args at(1) asNumber repeat(first receive(0))
writeln(sum)
*/


/*
times := System args at(1) asNumber
sum := 0
n := 0

send := method(
	if(n > 1, 
		n = n - 1
		coro := Coroutine clone setStackSize(20000)
		coro setRunTarget(Lobby) 
		coro setRunLocals(Lobby)
		coro setRunMessage(message(send))
		coro run
	, Lobby sum = sum + 500; mainCoro resume)
)
mainCoro := Coroutine currentCoroutine

iters := System args at(1) asNumber
for(i, 1, iters,
	n = 500
	send
	//writeln("iter ", i,  "\t", sum, "\t", (Collector maxAllocatedBytes/1000000) asString(0,2))
	//Collector collect
)
writeln(sum)
*/
