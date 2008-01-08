Curses do(
	inputBuffer := Sequence clone
	readLine := method(
		inputBuffer empty
		c := Curses asyncReadCharacter
		if(c) then(
			if(c == 13, break)
			inputBuffer append(c)
			Curses writeCharacter(c)
			Curses refresh
		) else(yield)
		inputBuffer
	)
)

