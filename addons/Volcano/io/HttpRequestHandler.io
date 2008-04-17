HttpRequestHandler := Object clone do(
	server ::= nil
	parser ::= nil
	
	init := method(
		setParser(HttpParser clone)
	)
	
	handleRequest := method(socket,
		//writeln("while(parser isFinished not,")
		while(parser isFinished not,
			//writeln("streamReadNextChunk")
			socket streamReadNextChunk ifError(e,
				writeln("Error reading next chunk: ", e message)
				completeRequest(socket)
				return
			)
			//debugWriteln("socket isOpen")
			socket isOpen ifFalse(
				writeln("Socket closed while reading next chunk.")
				completeRequest(socket)
				return
			)
			//writeln("parser setParseBuffer")
			parser setParseBuffer(socket readBuffer)
			//writeln("parser parse")
			parser parse ifError(e,
				writeln("Error parsing request: ", e message)
				completeRequest(socket)
				return
			)
		)
		//writeln("streamResponse")
		streamResponse(socket, parser httpRequest)
		//writeln("completeRequest")
		completeRequest(socket)
	)
	
	completeRequest := method(socket,
		parser reset
		socket close
		server completedRequest(self)
	)
	
	streamResponse := method(socket, request,
		HttpResponse withSocket(socket) setBody("<html>Hello</html>") send
	)
)