#!/usr/local/bin/ioServer

Curses do(
	begin
	clear
)

Lobby exit := method(Curses end; System exit)

loop(
	Curses move(0, 0) write("Io> ") refresh
	input := Sequence clone

	loop(
		c := Curses asyncReadCharacter
		if(c) then(
			if(c == 13, break) 
			input append(c)
			Curses writeCharacter(c)
			Curses refresh
		) else(yield)
	)
	
	Curses clear
	
	e := try(
		result := Lobby doString(input)
		Curses move(0, 1)
		Curses write("==> " .. getSlot("result") asSimpleString) refresh
	)
	
	e catch(Exception,
		Curses move(0, 1) write(e coroutine backTraceString)
	)
)

Curses end
