
EvHttpRequestHandler := Object clone do(
	handleRequest := method(request, response,
			writeln("EvHttpRequestHandler - you need to override this method")
	        response data = URL with("http://yahoo.com/") fetch size asString
			//response data := ""
			response statusCode := 200
			response responseMessage := "OK"
			response asyncSend
	)
)

EvOutResponse do(
	headers := Map clone
	statusCode := 200
	data := ""
	responseMessage := "OK" // "Internal Server Error"
)

EvHttpServer do(
	eventManager ::= EventManager
	host ::= "127.0.0.1"
	port ::= 80
	
	request := EvHttpRequest clone

	run := method(
		EventManager run
		start
		self
	)
	
	// response slot is set by EvHttpServer
	
	requestHandlerProto ::= EvHttpRequestHandler
	handleRequestCallback := method(
		response headers := Map clone
		request parse
		//writeln("parameters = ", request parameters keys)
		requestHandlerProto clone @handleRequest(request, response)
	)
	
	handleError := method(e,
		response data := "<pre>" .. e coroutine backTraceString .. "</pre>"
		response statusCode := 500
	)
)

