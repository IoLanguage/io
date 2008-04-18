HttpParser do(
	parseBuffer ::= ""

	init := method(
		self httpRequest := HttpRequest clone
	)
	
	reset := method(
		rawReset
		list(requestUri, fragment, requestPath, queryString, httpVersion, body, httpFields) foreach(empty)
		
		httpRequest setUri(requestUri)\
		setFragment(fragment)\
		setPath(requestPath)\
		setQueryString(queryString)\
		setVersion(httpVersion)\
		setBody(body)\
		setFields(httpFields)
	)
)