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
			resultsOrError returnIfError at("results")
	)
)