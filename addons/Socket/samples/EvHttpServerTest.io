URL with("http://www.yahoo.com/") fetch

MyRequestHandler := Object clone do(
	baseUri ::= "http://www.google.com" 
	requests := 0
    handleRequest := method(request, response,
		MyRequestHandler requests = MyRequestHandler requests + 1
		writeln(response requestId, " >>>> request ", request uri)
        response data = URL with(baseUri .. request uri) fetch 
		if(response data type == "Error", writeln(response data); response data = "error")
		response statusCode := 200
		response responseMessage := "OK"
		//writeln("  ", response requestId, " <<<< response ", request uri, " ", response data size)
		response asyncSend
		response headers = request headers
		MyRequestHandler requests = MyRequestHandler requests - 1
		writeln("open requests: ", MyRequestHandler requests)
    )
)

//URL useEv

EvHttpServer clone setHost("127.0.0.1") setPort(8080) setRequestHandlerProto(MyRequestHandler) run
//Coroutine currentCoroutine pause
loop(yield)