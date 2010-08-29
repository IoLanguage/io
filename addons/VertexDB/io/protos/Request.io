VertexDB Request := Object clone do(
	//internal 
	queryParams ::= Map clone
	queryParamNames ::= list("action")
	action ::= "size"
	
	shouldSend ::= true
	
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
		qnames foreach(name, self newSlot(name))
		self queryParamNames appendSeq(qnames)
		self
	)
	
	queryString := method(
		queryParamNames foreach(name,
			if(v := self getSlot(name),
				queryParams atPut(name, v asString)
			)
		)
		queryParams asQueryString
	)
	
	resource := method(
		path percentEncoded asMutable replaceSeq("%2F", "/") .. queryString
	)
	
	requestUrl := method(
		"http://" .. host .. ":" .. port asString .. resource
	)

	execute := method(
		dt := Date secondsToRun(
			url := URL with("http://" .. host .. ":" .. port asString .. resource) setFollowRedirects(false)
			if(VertexDB Settings username,
				url setUsername(VertexDB Settings username)
				url setPassword(VertexDB Settings password)
				url setUsesBasicAuthentication(true)
			)
		
			debugWriteln(url url)
			if(body size > 0,
				debugWriteln(body split("\n") map(line, "\t" .. line) join("\n"))
			)
		
			r := VertexDB Response clone setRequest(self)
			if(shouldSend,
				r setBody(if(httpMethod asLowercase == "get", url fetch, url post(body)))
			,
				r setBody("SENDING DISABLED")
			)
			
			r setStatusCode(url statusCode)
		)
		debugWriteln("dt: ", dt)
		r
	)
	
	results := method(
		response := execute
		raisesOnError = true
		
		if(raisesOnError, response raiseIfError)
		response results
	)

	key ::= nil
	value ::= nil
)

VertexDB do(
	ReadRequest := Request clone setAction("read") addQuerySlots(list("key", "mode"))
	
	SizeRequest := Request clone setAction("size")
	
	WriteRequest := Request clone setAction("write") addQuerySlots(list("key", "value", "mode"))
	WriteRequest queryString := method(Sequence with("?action=write&key=", key percentEncoded, "&value=", value percentEncoded))

	RmRequest := Request clone setAction("rm") addQuerySlots(list("key"))
	
	MkdirRequest := Request clone setAction("mkdir")
	MkdirRequest queryString := method(Sequence with("?action=mkdir"))

	TransactionRequest := Request clone setAction("transaction") setHttpMethod("post")
	TransactionRequest queryString := Sequence with("?action=transaction")
	
	QueryRequest := Request clone do(
		setAction("select")
		addQuerySlots(list("op", "before", "after", "count", "whereKey", "whereValue"))
		
		foreachBlock := method(b,
			last := ""
			loop(
				keys := self setOp("keys") setAfter(last) setCount(200) results
				if(keys isEmpty, break)
				keys foreach(k, b call(k, nodeAt(k)))
				last := keys last
			)
		)
		
		foreachAttributeBlock := method(attribute, b,
			last := ""
			loop(
				value := queryRequest setOp("value") setAfter(last) setAttribute(attribute) setCount(200) results
				if(keys isEmpty, break)
				keys foreach(k, b call(k, nodeAt(k)))
				last := keys last
			)
		)
	)
	
	LinkToRequest := Request clone do(
		setAction("link")
		addQuerySlots(list("fromPath", "toPath", "key"))
	)
	
	QueuePopToRequest := Request clone do(
		setAction("queuePopTo")
		addQuerySlots(list("toPath", "ttl", "whereKey", "whereValue"))
	)
	
	QueueExpireToRequest := Request clone do(
		setAction("queueExpireTo")
		addQuerySlots(list("toPath"))
	)
)