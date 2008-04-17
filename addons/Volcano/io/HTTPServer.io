HttpServer := Server clone do(
	setPort(8080)
	setHost("127.0.0.1")
	
	init := method(
		self recycledHandlers := List clone
	)
	
	handleSocket := method(socket,
		handler := recycledHandlers pop
		if(handler == nil, handler = HttpRequestHandler clone)
		//writeln("handler := HttpRequestHandler clone")
		//handler := HttpRequestHandler clone
		//writeln("handler setServer(self) @handleRequest(socket)")
		handler setServer(self) @handleRequest(socket)
	)
	
	completedRequest := method(handler, recycledHandlers append(handler))
)