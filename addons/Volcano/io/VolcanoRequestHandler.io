VolcanoRequestHandler := Object clone do(
	server ::= nil
	parser ::= nil
	socket ::= nil
	
	init := method(
		setParser(HttpParser clone)
	)
	
	handleRequest := method(
		while(parser isFinished not,
			Logger info("socket streamReadNextChunk")
			socket streamReadNextChunk ifError(e,
				?Logger warn("VolcanoRequestHandler error while parsing request: ", e message)
				return(self)
			)
			Logger info("socket isOpen ifFalse")
			socket isOpen ifFalse(
				?Logger warn("VolcanoRequestHandler socket closed before parse completion")
				return(self)
			)
			Logger info("parser parse")
			parser setParseBuffer(socket readBuffer)
			parser parse ifError(e,
				?Logger warn("VolcanoRequestHandler error parsing request: ", e message)
				response setStatusCode(400)
				response body := "Bad Request: " .. e message
				response send
				return(self)
			)
		)
		streamResponse
		self
	)
	
	streamResponse := method(
		response := HttpResponse withSocket(socket)
		e := try(server renderResponse(request, response))
		if(e,
			response setStatusCode(500)
			response body := "<pre>" .. e coroutine backTraceString .. "</pre>"
		)
		response send
	)
)