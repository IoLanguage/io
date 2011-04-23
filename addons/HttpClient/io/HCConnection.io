//metadoc HCConnection category Networking
//metadoc HCConnection copyright Rich Collins, 2010
//metadoc HCConnection license BSD revised
//metadoc HCConnection description Sends an HCRequest using the HTTP protcol and stores the response in an HCResponse

//doc HCConnection request An HCRequest describing the HTTP request to be sent
//doc HCConnection response An HCResponse describing the response received from the remote host
//doc HCConnection sendRequest Send the request and set the response

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
			request setHeader("Host", Sequence with(request host, ":", request port asString))
		)

		request headers select(k, v, v != nil) map(k, v,
			Sequence with(k, ": ", v)
		) join(lineTerminator)
	)
	
	httpMessage := method(
		Sequence with(statusLine, headersLines, headerTerminator, request body)
	)
	
	sendMessage := method(
		socket write(httpMessage) raiseIfError
	)
	
	readResponse := method(
		parser := HCResponseParser clone setData(socket readBuffer)
		
		while(socket isOpen,
			socket streamReadNextChunk raiseIfError
			parser parseMessageIfPossible
			if(parser contentReceived,
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
