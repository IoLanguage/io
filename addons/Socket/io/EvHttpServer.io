EvHttpServer do(
	eventManager ::= EventManager
	host ::= "127.0.0.1"
	port ::= 80
	
	request := EvHttpRequest clone
	
	response := EvHttpResponse clone
	
	handleRequestCallback := method(
		//writeln("HttpServer handleRequest not implemented 1")
		response headers := Map clone
		request parse
		//writeln("parameters = ", request parameters keys)
		self handleRequest(request)
		
		self
	)
	
	handleRequest := method(request,
		writeln("HttpServer handleRequest not implemented")
		response data := ""
		response statusCode := 200
	)
)
