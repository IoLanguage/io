OauthResponse := Object clone do(
	request ::= nil
	data ::= nil
	
	body ::= nil
	statusCode ::= nil
	headers ::= nil
	
	init := method(
		setHeaders(Map clone)
	)
	
	parseData := method(
		debugWriteln(data)
		headersSeq := data beforeSeq("\n\n")
		if(headersSeq isEmpty,
			Exception raise("Empty Response for " .. request url .. ":\n" .. data)
		)
		lines := headersSeq split("\n")
		setStatusCode(lines removeFirst betweenSeq(" ", " ") asNumber)
		lines foreach(headerLine,
			headers atPut(headerLine beforeSeq(":"), headerLine afterSeq(": "))
		)

		setBody(data afterSeq("\n\n"))
	)
	
	formEncodedMap := method(
		outMap := Map clone
        body split("&") foreach(kv, 
            kv = kv split("=")
            outMap atPut(kv first, kv second)
        )
        outMap
	)
)