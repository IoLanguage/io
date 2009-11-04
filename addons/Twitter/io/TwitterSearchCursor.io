TwitterSearchCursor := Object clone do(
	search ::= nil
	
	index ::= -1
	results ::= nil
	
	init := method(
		setResults(List clone)
	)	
	
	next := method(
		if(index == results size - 1) then(
			index = index + 1
			fetchNext
		) else(
			index = index + 1
		)
		
		result
	)
	
	result := method(
		results at(index)
	)
	
	fetchNext := method(
		search setPage(search page + 1) handleErrors(results) else(searchResults, results appendSeq(searchResults))
		self
	)
)