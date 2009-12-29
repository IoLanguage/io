//URL useEv

writeln("initial fetch")
URL with("http://www.yahoo.com/") fetch

writeln("running")
MyRequestHandler := Object clone do(
	baseUri ::= "http://www.google.com" 
	requests := 0
    handleRequest := method(request, response,
		//writeln(response requestId, " >>>> request ", request uri)
		MyRequestHandler requests = MyRequestHandler requests + 1
        response data = URL with(baseUri .. request uri) fetch 
		MyRequestHandler requests = MyRequestHandler requests - 1
		if(response data type == "Error", writeln(response data); response data = "error")
		response statusCode := 200
		response responseMessage := "OK"
		//writeln("  ", response requestId, " <<<< response ", request uri, " ", response data size)
		response asyncSend
		response headers = request headers
		writeln("open requests: ", MyRequestHandler requests)
    )
)


EvHttpServer clone setHost("127.0.0.1") setPort(8080) setRequestHandlerProto(MyRequestHandler) run
Coroutine currentCoroutine pause
loop(yield)