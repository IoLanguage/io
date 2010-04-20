//metadoc HCResponseParser category Networking
//metadoc HCResponseParser copyright Rich Collins, 2010
//metadoc HCResponseParser license BSD revised
//metadoc HCResponseParser description Handles parsing response received during an HCConnection

HCResponseParser := Object clone do(
	data ::= nil
	response ::= nil
	
	lineTerminator ::= "\r\n"
	headerTerminator ::= (lineTerminator repeated(2))
	
	parseMessageIfPossible := method(
		if(response == nil and data containsSeq(headerTerminator),
			setResponse(HCResponse clone)
			parseMessage
		)
		self
	)
	
	parseMessage := method(
		parseStatus
		parseHeaders
	)
	
	parseStatus := method(
		statusParts := data beforeSeq(lineTerminator) split(" ")
		response setStatusCode(statusParts at(1))
		response setStatusDescription(statusParts at(2))
	)
	
	messageWasParsed := method(
		response != nil
	)
	
	parseHeaders := method(
//data afterSeq(lineTerminator) beforeSeq(headerTerminator) println
		data afterSeq(lineTerminator) beforeSeq(headerTerminator) split(lineTerminator) foreach(headerLine,
			response addHeader(headerLine beforeSeq(":"), headerLine afterSeq(":") strip)
		)
		self
	)
	
	parseContent := method(
		if(response,
			if(data containsSeq(headerTerminator) not,
				Exception raise("Malformed HttpResponse.  No header terminator.")
			)

			response setContent(data afterSeq(headerTerminator))
		)
		self
	)
	
	contentReceived := method(
		if(response,
			contentLength := response headerAt("Content-Length")
			if(contentLength,
				contentLength asNumber == parseContent response content size
			,
				false
			)
		,
			false
		)
	)
)