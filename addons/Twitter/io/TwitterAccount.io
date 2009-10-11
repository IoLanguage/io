TwitterAccount := Object clone do(
	screenName ::= nil
	password ::= nil
	
	profile ::= nil
	
	init := method(
		setProfile(TwitterAccountProfile clone setAccount(self))
	)
	
	request := method(name,
		TwitterRequest clone setUsername(screenName) setPassword(password)
	)
	
	doRequest := method(
		requestName := call message arguments first name
		request := requestNamed(requestName)
		
		call sender setSlot(requestName, request)
		call evalArgAt(1)
		results := request results
		if(error := results at("error"), Error with(error), results)
	)
	
	hasFriend := method(screenName,
		//Could not find target user.
		request asShowFriendship setTargetScreenName(screenName) resultsOrError\
			returnIfError\
			at("relationship")\
				at("source")\
					at("following")
	)
	
	follow := method(screenName,
		//Could not follow user: richcollins is already on your list.
		//Could not follow user: You have been blocked from following this account at the request of the user.
		//Could not follow user: This account is currently suspended and is being investigated due to strange activity
		
		request asCreateFriendship setScreenName(screenName) resultsOrError returnIfError
		self
	)
	
	unfollow := method(screenName,
		//You are not friends with the specified user
		
		request asDestroyFriendship setScreenName(screenName) resultsOrError returnIfError
		self
	)
	
	followerIds := method(
		//"Not authorized"
		request asFollowerIds setScreenName(screenName) resultsOrError returnIfError
	)
	
	friendIds := method(
		//"Not authorized"
		request asFriendIds setScreenName(screenName) resultsOrError returnIfError
	)
	
	updateStatus := method(message,
		request asUpdateStatus setStatus(message) resultsOrError returnIfError
		self
	)
)

//TwitterRequest debugOn