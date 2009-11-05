TwitterSearch := Object clone do(
	query ::= nil
	perPage ::= 100
	sinceUpdateId ::= nil
	page ::= nil
	
	request := method(name,
		TwitterRequest clone
	)
	
	results := method(
		//Could not find target user.
		request asSearch\
			setQ(query)\
			setRpp(perPage)\
			setSinceId(sinceUpdateId)\
			setPage(page)\
			execute raiseIfError results at("results")
	)
	
	cursor := method(
		TwitterSearchCursor clone setSearch(self clone setPage(0))
	)
	
	handleErrors := method( //TODO: Unify this with account
		attempts := 0

		while(attempts < 3,
			e := try(
				result := self doMessage(call message arguments at(0), call sender)
			)

			if(e,
				if(e hasProto(TwitterException),
					if(e isOverloaded or e isDown or e isInternalError,
						attempts = attempts + 1
					,
						return(TwitterAccount ExceptionConditional clone setException(e))
					)
				,
					if(list("Connection reset by peer", "Timeout") detect(m, e error containsSeq(m)),
						attempts = attempts + 1
					,
						e pass
					)
				)
			,
				return(TwitterAccount ExceptionConditional clone setResult(result) setDone(true))
			)
		)

		e pass
	)
)