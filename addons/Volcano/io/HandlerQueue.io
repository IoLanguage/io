HandlerQueue := Object clone do(
	server ::= nil
	coro ::= nil
	paused ::= false
	
	init := method(
		self recycledHandlers := List clone
		self queue := List clone
		self inProcess := 0
		self concurrencyLimit := 300
		self recyclingEnabled := false
	)
	
	processQueue := method(
		setCoro(Coroutine currentCoroutine)
		loop(
			if(queue isEmpty not and inProcess < concurrencyLimit) then(
				handler := queue removeAt(0)
				handler @handleRequest
				inProcess = inProcess + 1
			) else(
				if(paused not,
					setPaused(true)
					?Logger info("Volcano HandlerQueue is full (", concurrencyLimit, ").")
				)
				coro pause
			)
		)
	)
	
	enqueue := method(socket,
		if(recyclingEnabled) then(
			handler := recycledHandlers pop
		) else(
			handler := nil
		)
		
		handler ifNil(handler = HttpRequestHandler clone setHandlerQueue(self))
		handler setSocket(socket)
		if(inProcess < concurrencyLimit) then(
			handler @handleRequest
			inProcess = inProcess + 1
		) else(
			queue append(handler)
			if(paused,
				setPaused(false)
				?Logger info("Volcano HandlerQueue resume")
				coro resume
			)
		)
	)
	
	requestCompleted := method(handler,
		recyclingEnabled ifTrue(
			handler reset
			recycledHandlers append(handler)
		)
		inProcess = inProcess - 1
		server ?requestCompleted
	)
)