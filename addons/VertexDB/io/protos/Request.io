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
	host ::= method(VertexDB Settings host)
	port ::= method(VertexDB Settings port)
	path ::= "/" asMutable
	raisesOnError ::= true
	
	httpMethod ::= "get"
	body ::= ""

	addQuerySlots := method(qnames,
		self queryParamNames appendSeq(qnames)
		self
	)
	
	queryString := method(
		queryParamNames foreach(name,
			if(v := self getSlot(name),
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
		
		r := VertexDB Response clone setRequest(self)\
			setBody(if(httpMethod asLowercase == "get", url fetch, url post(body)))\
			setStatusCode(url statusCode)

		//writeln("url statusCode: ", url statusCode)
		r
	)
	
	results := method(
		response := execute
		if(raisesOnError, response raiseIfError)
		response results
	)

	key ::= nil
	value ::= nil
)

VertexDB do(
ReadRequest := VertexDB Request clone setAction("read") addQuerySlots(list("key")) setHttpMethod("get")

SizeRequest := VertexDB Request clone setAction("size") setHttpMethod("get")
WriteRequest := VertexDB Request clone setAction("write")\
			addQuerySlots(list("key", "value"))\
			setHttpMethod("get")
WriteRequest queryString := method(Sequence with("?action=write&key=", key, "&value=", value))

RmRequest := VertexDB Request clone setAction("rm") addQuerySlots(list("key")) setHttpMethod("get")	
MkdirRequest := VertexDB Request setAction("mkdir") setHttpMethod("get")
MkdirRequest queryString := method(Sequence with("?action=mkdir"))

TransactionRequest := Request clone\
				setHttpMethod("get")\
				setAction("transaction")
TransactionRequest queryString := Sequence with("?action=transaction")
SelectRequest Request clone do(
				setHttpMethod("post")
				setAction("select")
				op ::= nil
				before ::= nil
				after ::= nil
				count ::= nil
				whereKey ::= nil
				whereValue ::= nil
)

)
