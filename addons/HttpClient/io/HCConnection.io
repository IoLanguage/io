HCConnection := Object clone do(
	request ::= nil
	response ::= nil
	
	socket ::= nil
	
	responseData ::= nil
	
	openSocket := method(
		setSocket(Socket clone)
		socket setHost(request host)
		socket setPort(request port)
		socket connect raiseIfError
	)
	
	lineTerminator ::= "\r\n"
	headerTerminator ::= (lineTerminator repeated(2))
	
	statusLine := method(
		list(request httpMethod, request resource, "HTTP/1.1") join(" ") appendSeq(lineTerminator)
	)
	
	headersLines := method(
		if(request port asString == "80",
			request setHeader("Host", request host)
		,
			request setHeader("Host", Sequence with(request host, ":", request port))
		)

		request headers select(k, v, v != nil) map(k, v,
			Sequence with(k, ": ", v)
		) join(lineTerminator)
	)
	
	httpMessage := method(
		Sequence with(statusLine, headersLines, headerTerminator)
	)
	
	sendMessage := method(
		httpMessage println
		socket write(httpMessage) raiseIfError
	)
	
	readResponse := method(
		parser := HCResponseParser clone setData(socket readBuffer)
		
		while(socket isOpen,
			socket streamReadNextChunk raiseIfError
			parser parseMessageIfPossible
			if(parser contentReceived,
				"contentReceived close socket" println
				socket close
			)
		)
		
		parser parseContent
		setResponse(parser response)
	)
	
	sendRequest := method(
		if(socket,
			Exception raise("Request already sent")
		)
		
		openSocket
		sendMessage
		readResponse
		
		self
	)
)