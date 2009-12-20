EvHttpServer do(
	eventManager ::= EventManager
	host ::= "127.0.0.1"
	port ::= 80
	
	request := EvHttpRequest clone
	
	response := EvHttpResponse clone
	
	handleRequestCallback := method(
		writeln("EvHttpServer handleRequestCallback START")
		response = EvHttpResponse clone
		request parse
		//writeln("parameters = ", request parameters keys)
		e := try(self handleRequest(request))
		if(e,
			handleError(e)
		)
		writeln("response data size: ", response data size)
		writeln("EvHttpServer handleRequestCallback END")
		self
	)
	
	handleRequest := method(request,
		writeln("HttpServer handleRequest not implemented")
		response data = ""
		response statusCode := 200
	)
	
	handleError := method(e,
		response data := "<pre>" .. e coroutine backTraceString .. "</pre>"
		response statusCode := 500
	)
)
