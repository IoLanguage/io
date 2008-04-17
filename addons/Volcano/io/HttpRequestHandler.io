HttpRequestHandler := Object clone do(
	handlerQueue ::= nil
	parser ::= nil
	socket ::= nil
	
	init := method(
		setParser(HttpParser clone)
	)
	
	handleRequest := method(
		while(parser isFinished not,
			socket streamReadNextChunk ifError(e,
				completeRequest(socket)
				return
			)
			socket isOpen ifFalse(
				writeln("Socket closed while reading next chunk.")
				completeRequest(socket)
				return
			)
			parser setParseBuffer(socket readBuffer)
			parser parse ifError(e,
				writeln("Error parsing request: ", e message)
				completeRequest(socket)
				return
			)
		)
		streamResponse(socket, parser httpRequest)
		completeRequest(socket)
	)
	
	completeRequest := method(socket,
		parser reset
		socket close
		handlerQueue requestCompleted(self)
	)
	
	streamResponse := method(socket, request,
		HttpResponse withSocket(socket) setBody("<html>Hello</html>") send
	)
)