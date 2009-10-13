VertexDB Request := Object clone do(
	//internal 
	queryParams ::= Map clone
	queryParamNames ::= list("action")
	action ::= "size"
	
	init := method(
		setQueryParams(queryParams clone)
		setQueryParamNames(queryParamNames clone)
		setPath(path clone)
	)
	
	//api
	host ::= method(Settings host)
	port ::= method(Settings port)
	path ::= "/" asMutable
	raisesOnError ::= true
	
	httpMethod ::= "get"
	body ::= ""

	addQuerySlots := method(querySlotNames,
		querySlotNames split foreach(name,
			self newSlot(name)
			queryParamNames appendIfAbsent(name)
		)
		self
	)
	
	queryString := method(
		queryParamNames foreach(name,
			if(v := self perform(name asSymbol),
				queryParams atPut(name, v asString)
			)
		)
		"?" .. queryParams asQueryString
	)
	
	resource := method(
		path .. queryString
	)

	execute := method(
		url := URL with("http://" .. host .. ":" .. port asString .. resource) setFollowRedirects(false)
		
		debugWriteln(url url)
		if(body size > 0,
			debugWriteln(body split("\n") map(line, "\t" .. line) join("\n"))
		)
		
		Response clone\
			setBody(if(httpMethod asLowercase == "get", url fetch, url post(body)))\
			setStatusCode(url statusCode)
	)
	
	results := method(
		response := execute
		if(raisesOnError, response raiseIfError)
		response results
	)
)