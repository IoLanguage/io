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
			results at("results")
	)
	
	cursor := method(
		TwitterSearchCursor clone setSearch(self clone setPage(0))
	)
)