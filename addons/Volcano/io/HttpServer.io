HttpServer := Server clone do(
	setPort(8080)
	setHost("127.0.0.1")
	
	init := method(
		self handlerQueue := HandlerQueue clone setServer(self)
	)
	
	start := method(
		handlerQueue @processQueue
		resend
	)
	
	handleSocket := method(socket,
		handlerQueue enqueue(socket)
	)
)