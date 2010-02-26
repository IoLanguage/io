TwitterSearch := Object clone do(
	//metadoc TwitterSearch category Networking
	
	query ::= nil
	//doc TwitterSearch query Returns the search query.
	//doc TwitterSearch setQuery(aSeq) Sets the query. Returns self.
	
	perPage ::= 100
	//doc TwitterSearch perPage Returns the results per page.
	//doc TwitterSearch setPerPage(aSeq) Sets the results per page. Returns self.

	sinceUpdateId ::= nil
	//doc TwitterSearch sinceUpdateId Returns the sinceUpdateId.
	
	page ::= nil
	//doc TwitterSearch sinceUpdateId Returns the current page number of search results.
	
	request := method(name,
		TwitterRequest clone
	)
	
	results := method(
		//doc TwitterSearch results Returns the results for the current page.
		//Could not find target user.
		request asSearch\
			setQ(query)\
			setRpp(perPage)\
			setSinceId(sinceUpdateId)\
			setPage(page)\
			execute raiseIfError results at("results")
	)
	
	cursor := method(
		//doc TwitterSearch cursor Returns the TwitterSearchCursor for the first page.
		TwitterSearchCursor clone setSearch(self clone setPage(0))
	)
)