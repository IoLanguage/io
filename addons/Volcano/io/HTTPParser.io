HttpParser do(
	parseBuffer ::= ""
	
	asRequest := method(
		HttpRequest clone setUri(?requestURI)\
			setFragment(?fragment)\
			setPath(?requestPath)\
			setQueryString(?queryString)\
			setVersion(?HttpVersion)\
			setBody(?body)\
			setFields(HttpFields)
	)
)