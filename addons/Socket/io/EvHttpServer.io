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
	
	response := Object clone do(
		headers := Map clone
		statusCode := 200
		data := ""
	)
	
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
