HandlerQueue := Object clone do(
	init := method(
		self recycledHandlers := List clone
		self queue := List clone
		self inProcess := 0
		self concurrencyLimit := 200
	)
	
	processQueue := method(
		loop(
			if(queue isEmpty not and inProcess < concurrencyLimit) then(
				handler := queue removeAt(0)
				handler @handleRequest
				inProcess = inProcess + 1
			) else(
				yield
			)
		)
	)
	
	enqueue := method(socket,
		handler := recycledHandlers pop
		handler ifNil(handler = HttpRequestHandler clone setHandlerQueue(self))
		handler setSocket(socket)
		queue append(handler)
	)
	
	requestCompleted := method(handler,
		recycledHandlers append(handler)
		inProcess = inProcess - 1
	)
)