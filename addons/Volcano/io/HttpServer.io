HttpServer := Server clone do(
	setPort(8080)
	setHost("127.0.0.1")
	
	init := method(
		
		recycledRequests := List clone
	)
	
	handleSocket := method(socket,
		request := recycledRequests pop
		if(request == nil, request = HttpRequest clone)
		request setServer(self) @handleSocket(socket)
	)
	
	completedRequest := method(request, recycledRequests append(request))
)