
Server clone do(
	setPort(8000)
	handleSocket := method(s,
		while(s streamReadNextChunk,
			writeln(s readBuffer)
			s readBuffer empty
		)
	)
) start

// http://localhost:8000/stream/1018

