Yajl

VertexDB Response := Object clone do(
	//api
	body ::= nil
	statusCode ::= nil
	request ::= nil

	results ::= method(
		setResults(Yajl parseJson(body))
		results
	)
	
	raiseIfError := method(
		body ifError(e,
			Exception raise(e message)
		)
		if(statusCode == 500) then(
			//writeln("exception!!!!!!!!!!")
			Exception raise("VertexDB Error: Status 500 '" .. results .. "' on resource: " .. request resource .. " " .. request body)
		)
	)
)