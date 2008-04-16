HttpServer := Server clone do(
	setPort(8080)
	setHost("127.0.0.1")

	handleSocket := method(socket,
		@handleRequest(socket)
	)
	
	handleRequest := method(socket,
		parser := HttpParser clone setParseBuffer(socket readBuffer)
		while(parser isFinished not,
			socket streamReadNextChunk ifError(e,
				writeln("Error reading next chunk: ", e description)
				socket close
				return
			)
			parser parse ifError(e,
				writeln("Error parsing request: ", e description)
				socket close
				return
			)
		)
		streamResponse(socket, parser asRequest)
		socket close
	)
	
	streamResponse := method(socket, request,
		HttpResponse withSocket(socket) setBody("<html>Hello</html>") send
	)
)