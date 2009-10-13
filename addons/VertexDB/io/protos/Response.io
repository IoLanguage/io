Yajl

VertexDB Response := Object clone do(
	//api
	body ::= nil
	statusCode ::= nil
	results ::= method(
		setResults(Yajl parseJson(body))
		results
	)
	
	raiseIfError := method(
		body ifError(e,
			Exception raise(e message)
		)
		if(statusCode == 500) then(
			Exception raise("VertextDB Error: " .. results)
		)
	)
)