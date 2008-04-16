HTTPParser do(
	parseBuffer ::= ""
	
	asRequest := method(
		HTTPRequest clone setUri(?requestURI)\
			setFragment(?fragment)\
			setPath(?requestPath)\
			setQueryString(?queryString)\
			setVersion(?httpVersion)\
			setBody(?body)\
			setFields(httpFields)
	)
)