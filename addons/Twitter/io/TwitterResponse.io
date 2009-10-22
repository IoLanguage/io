Yajl

TwitterResponse := Object clone do(
	//api
	body ::= nil
	statusCode ::= nil
	request ::= nil
	rateLimitRemaining ::= nil
	rateLimitExpiration ::= nil

	results ::= method(
		setResults(Yajl parseJson(body))
		results
	)
	
	raiseIfError := method(
		body ifError(e,
			Exception raise(e message)
		)
		
		if(statusCode == 400) then(
			TwitterBadRequestException raise(body)
		) elseif(statusCode == 401) then(
			TwitterNotAuthorizedException raise("bad username / password")
		) elseif(statusCode == 403) then(
			TwitterForbiddenException raise(body)
		) elseif(statusCode == 404) then(
			TwitterNotFoundException raise
		) elseif(statusCode == 500) then(
			TwitterInternalErrorException raise(body)
		) elseif(statusCode == 502) then(
			TwitterDownException raise
		) elseif(statusCode == 503) then(
			TwitterOverloadedException raise
		)
		
		results ifError(
			Exception raise(e message)
		)
		
		if(results at("error"),
			TwitterUnknownException raise(results at("error"))
		)
		
		
		self
	)
)