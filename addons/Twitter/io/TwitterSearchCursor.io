TwitterSearchCursor := Object clone do(
	//metadoc TwitterSearchCursor category Networking
	
	search ::= nil
	//metadoc TwitterSearchCursor search Returns the search string.
	
	index ::= -1
	//metadoc TwitterSearchCursor index Index of cursor pointer.
	
	results ::= nil
	//metadoc TwitterSearchCursor results Returns the list of results.
	
	init := method(
		setResults(List clone)
	)	
	
	next := method(
		//metadoc TwitterSearchCursor next Moves the index to the next result and returns it.
		if(index == results size - 1) then(
			index = index + 1
			fetchNext
		) else(
			index = index + 1
		)
		
		result
	)
	
	result := method(
		//metadoc TwitterSearchCursor Returns the current result or nil if there are no more results.
		results at(index)
	)
	
	fetchNext := method(
		//metadoc TwitterSearchCursor fetchNext Fetches the next page of results. Returns self.
		results appendSeq(search setPage(search page + 1) results)
		self
	)
)