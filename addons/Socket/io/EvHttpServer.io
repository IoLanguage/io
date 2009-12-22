Sequence do(
	parsedAsQueryParameters := method(
		params := Map clone
		splitNoEmpties("&") foreach(i, v,
			key := v beforeSeq("=")
			value := v afterSeq("=")
			
			params atIfAbsentPut(key, List clone)
			params at(key) append(URL unescapeString(value))
		)
		params
	)
)

EvInRequest := Object clone do(
	headers := Map clone
	uri := nil
	postData := nil
	query := nil
	path := nil
	parameters := nil
	cookies := nil
	
	init := method(
		headers = headers clone
	)
	
	
	parse := method(
		parseUri
		parseHeaders
		self
	)
	
	parseUri := method(
		path = uri beforeSeq("?")
		query = uri afterSeq("?")
		parseQuery
	)
	
	parseQuery := method(
		parameters = if(query, query parsedAsQueryParameters, Map clone)
	)
	
	parseHeaders := method(
		parseCookies
	)
	
	parseCookies := method(
		raw := headers at("cookie")
		cookies = Map clone

		raw ?splitNoEmpties(";") foreach(cook,
			cook strip
			cookies atPut(cook beforeSeq("=") strip, URL unescapeString(cook afterSeq("=") strip))
		)
	)
)

// --------------------------------------------------------

EvInHttpRequestHandler := Object clone do(
	handleRequest := method(request, response,
			writeln("EvHttpRequestHandler - you need to override this method")
	        response data = URL with("http://yahoo.com/") fetch size asString
			//response data := ""
			response statusCode := 200
			response responseMessage := "OK"
			response asyncSend
	)
)

EvHttpServer do(
	eventManager ::= EventManager
	host ::= "127.0.0.1"
	port ::= 80
	
	requestProto := EvInRequest 
	responseProto := EvOutResponse

	run := method(
		EventManager resumeIfNeeded
		start
		self
	)
	
	// response slot is set by EvHttpServer
	
	requestHandlerProto ::= EvInHttpRequestHandler
	handleRequestCallback := method(
		response headers := Map clone
		request parse
		//writeln("parameters = ", request parameters keys)
		requestHandlerProto clone @@handleRequest(request, response)
	)
	
	handleError := method(e,
		response data := "<pre>" .. e coroutine backTraceString .. "</pre>"
		response statusCode := 500
	)
)

