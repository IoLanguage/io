HandlerQueue := Object clone do(
	server ::= nil
	coro ::= nil
	paused ::= false
	concurrencyLimit ::= 300
	
	init := method(
		self recycledHandlers := List clone
		self queue := List clone
		self inProcess := 0
		self recyclingEnabled := false
	)
	
	processQueue := method(
		setCoro(Coroutine currentCoroutine)
		loop(
			writeln("processQueue")
			if(queue isEmpty not and inProcess < concurrencyLimit) then(
				handler := queue removeAt(0)
				writeln("processQueue handleRequest")
				handler @handleRequest
				inProcess = inProcess + 1
			) else(
				if(paused not,
					setPaused(true)
				)
				writeln("pause")
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
		
		handler ifNil(handler = HttpRequestHandler clone)
		handler setHandlerQueue(self)
		handler setSocket(socket)
		if(inProcess < concurrencyLimit) then(
			writeln("enqueue handleRequest")
			inProcess = inProcess + 1
			handler @handleRequest
		) else(
			writeln("enqueue")
			queue append(handler)
			?Logger info("Volcano HandlerQueue is full: ", concurrencyLimit, " active and ", queue size, " queued")
			resumeIfNeeded
		)
	)
	
	resumeIfNeeded := method(
		if(paused,
			setPaused(false)
			coro resume
		)
	)
	
	requestCompleted := method(handler,
		recyclingEnabled ifTrue(
			handler reset
			recycledHandlers append(handler)
		)
		writeln("requestCompleted")
		inProcess = inProcess - 1
		resumeIfNeeded
		server ?requestCompleted
	)
)