TwitterFriendsFollowersCursor := Object clone do(
	account ::= nil
	screenName ::= nil
	nextId ::= "-1"
	fetchedIds ::= List clone
	index ::= -1
	requestType ::= nil
	
	init := method(
		fetchedIds = fetchedIds clone
	)
	
	fetchNext := method(
		result := account resultsFor(account request perform(requestType) setScreenName(screenName) setCursor(nextId))
		setNextId(result at("next_cursor_str"))
		fetchedIds appendSeq(result at("ids"))
	)
	
	next := method(
		index = index + 1
		if(index >= fetchedIds size,
			if(nextId == "0",
				nil
			,
				fetchNext
				id
			)
		,
			id
		)
	)
	
	id := method(
		fetchedIds at(index)
	)
)