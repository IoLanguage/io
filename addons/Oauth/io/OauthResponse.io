OauthResponse := Object clone do(
	data ::= nil
	
	body ::= nil
	statusCode ::= nil
	headers ::= nil
	
	init := method(
		setHeaders(Map clone)
	)
	
	parseData := method(
		//writeln("[", data, "]")
		headersSeq := data beforeSeq("\n")
		lines := headersSeq split("\n")
		setStatusCode(lines removeFirst betweenSeq(" ", " "))
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