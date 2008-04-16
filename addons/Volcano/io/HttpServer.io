HttpServer := Server clone do(
	setPort(8080)
	setHost("127.0.0.1")

	handleSocket := method(socket,
		HTTPRequest clone setServer(self) @handleRequest(socket)
	)
	
	streamResponse := method(socket, request,
		HttpResponse withSocket(socket) setBody("<html>Hello</html>") send
	)
)