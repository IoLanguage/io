TwitterAccount := Object clone do(
	screenName ::= nil
	password ::= nil
	
	profile ::= nil
	source ::= "API"
	
	rateLimitRemaining ::= nil
	rateLimitExpiration ::= nil
	
	init := method(
		setProfile(TwitterAccountProfile clone setAccount(self))
	)
	
	request := method(name,
		TwitterRequest clone setUsername(screenName) setPassword(password)
	)
	
	resultsFor := method(request,
		request execute
		
		if(request response rateLimitRemaining,
			setRateLimitRemaining(request response rateLimitRemaining asNumber)
		)
		
		if(request response rateLimitExpiration,
			setRateLimitExpiration(Date clone fromNumber(request response rateLimitExpiration asNumber))
		)
		
		request results
	)
	
	hasFriend := method(screenName,
		//Could not find target user.
		resultsFor(request asShowFriendship setTargetScreenName(screenName)) at("relationship") at("source") at("following")
	)
	
	follow := method(screenName,
		//Could not follow user: richcollins is already on your list.
		//Could not follow user: You have been blocked from following this account at the request of the user.
		//Could not follow user: This account is currently suspended and is being investigated due to strange activity
		
		resultsFor(request asCreateFriendship setScreenName(screenName))
		self
	)
	
	unfollow := method(screenName,
		//You are not friends with the specified user
		
		resultsFor(request asDestroyFriendship setScreenName(screenName))
		self
	)
	
	followerIds := method(
		//"Not authorized"
		resultsFor(request asFollowerIds setScreenName(screenName))
	)
	
	friendIds := method(
		//"Not authorized"
		resultsFor(request asFriendIds setScreenName(screenName))
	)
	
	updateStatus := method(message,
		resultsFor(request asUpdateStatus setStatus(message) setSource(source)) at("id")// asString
	)
	
	show := method(
		resultsFor(request asShow setScreenName(screenName))
	)
)