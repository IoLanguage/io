EvHttpRequestHandler := Object clone do(
	handleRequest := method(request, response,
			writeln("EvHttpRequestHandler need to override this method")
	        response data = URL with("http://dekorte.com/") fetch size asString
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
	
	request := Object clone do(
		headers := Map clone
		uri := nil
		postData := nil
		parse := method(
			self path := uri beforeSeq("?")
			q := uri afterSeq("?")
			self parameters := Map clone
			if(q,
				q split("&") foreach(p,
					parts := p split("=")
					parameters atPut(parts at(0), parts at(1))
				)
			)
		)
	)
	
	// response slot is set by EvHttpServer
	
	requestHandlerProto := EvHttpRequestHandler
	handleRequestCallback := method(
		response headers := Map clone
		request parse
		//writeln("parameters = ", request parameters keys)
		requestHandlerProto clone @handleRequest(request, response)
	)
)

