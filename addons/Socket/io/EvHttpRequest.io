Sequence do(
	parsedAsQueryParameters := method(
		params := Map clone
		splitNoEmpties("&") foreach(i, v,
			key := v beforeSeq("=")
			value := v afterSeq("=")
			
			params atIfAbsentPut(key, List clone)
			params at(key) append(URL unescapeString(value))
		)
		params
	)
)

EvHttpRequest := Object clone do(
	headers := Map clone
	uri := nil
	postData := nil
	query := nil
	path := nil
	parameters := nil
	cookies := nil
	
	init := method(
		headers = headers clone
	)
	
	
	parse := method(
		parseUri
		parseHeaders
		self
	)
	
	parseUri := method(
		path = uri beforeSeq("?")
		query = uri afterSeq("?")
		parseQuery
	)
	
	parseQuery := method(
		parameters = if(query, query parsedAsQueryParameters, Map clone)
	)
	
	parseHeaders := method(
		parseCookies
	)
	
	parseCookies := method(
		raw := headers at("cookie")
		cookies = Map clone

		raw ?splitNoEmpties(";") foreach(cook,
			cook strip
			cookies atPut(cook beforeSeq("=") strip, URL unescapeString(cook afterSeq("=") strip))
		)
	)
)