HttpParser do(
	parseBuffer ::= ""

	init := method(
		self httpRequest := HttpRequest clone
	)
	
	reset := method(
		rawReset
		list(requestUri, fragment, requestPath, queryString, httpVersion, body, httpFields) foreach(empty)
		fillHttpRequest
	)
	
	parse := method(
		rawParse
		fillHttpRequest
	)
	
	fillHttpRequest := method(
		httpRequest setUri(requestUri)\
			setFragment(fragment)\
			setPath(requestPath)\
			setQueryString(queryString)\
			setVersion(httpVersion)\
			setRequestMethod(requestMethod asCapitalized)\
			setBody(body)\
			setFields(httpFields)
	)
)