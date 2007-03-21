
writeln("start")

ackWithLoop := method(m, n,
	loop(
		//writeln(m, " ", n)
		if (m < 1, return n + 1)
		if (n < 1) then( 
			m = m - 1
			n = 1
		) else (
			n = ackWithLoop(m, n - 1) 
			m = m - 1
		)
	)
)

//ackWithLoop(3, 4) println

ackWithTailcalls := method(m, n,
	writeln(m, " ", n)
	if(m < 1, return n + 1)
	if(n < 1) then( 
		m = m - 1
		n = 1
		tailCall(m, n)
	) else(
		n = ackWithTailcalls(m, n - 1)
		m = m - 1
		tailCall(m, n)
	)
)

ackWithTailcalls(3, 4) println
