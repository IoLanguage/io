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
	
	isLimited := method(
		if(rateLimitRemaining == nil,
			updateRateLimits
		)
		rateLimitRemaining == 0
	)
	
	request := method(name,
		TwitterRequest clone setUsername(screenName) setPassword(password)
	)
	
	resultsFor := method(request,
		if(isLimited,
			TwitterException clone setIsRateLimited(true) raise("Rate Limited")
		)
		request execute
		debugWriteln(request response body)
		debugWriteln(request response statusCode)
		
		if(request response rateLimitRemaining,
			setRateLimitRemaining(request response rateLimitRemaining asNumber)
		)
		
		if(request response rateLimitExpiration,
			setRateLimitExpiration(Date clone fromNumber(request response rateLimitExpiration asNumber))
		)
		
		request response raiseIfError results
	)
	
	updateRateLimits := method(
		r := request asRateLimitStatus execute raiseIfError results
		
		setRateLimitRemaining(r at("remaining_hits") asNumber)
		setRateLimitExpiration(Date clone fromNumber(r at("reset_time_in_seconds") asNumber))
		self
	)
	
	hasFriend := method(aScreenName,
		//Could not find target user.
		resultsFor(request asShowFriendship setTargetScreenName(aScreenName)) at("relationship") at("source") at("following")
	)
	
	hasFollower := method(aScreenName,
		//Could not find target user.
		resultsFor(request asFriendshipExists setUserA(aScreenName) setUserB(screenName))
	)
	
	hasProtectedUpdates := method(aScreenName,
		showUser(aScreenName) at("protected")
	)
	
	/* for testing
	raiseFollowException := method(
		//self raiseFollowException := nil
		TwitterException clone setIsFollowLimit(true) raise("Follow limit reached")
	)
	*/
	
	follow := method(aScreenName,
		//Could not follow user: richcollins is already on your list.
		//Could not follow user: You have been blocked from following this account at the request of the user.
		//Could not follow user: This account is currently suspended and is being investigated due to strange activity
		//raiseFollowException for testing
		resultsFor(request asCreateFriendship setScreenName(aScreenName)) at("protected")
	)
	
	unfollow := method(aScreenName,
		//You are not friends with the specified user
		
		resultsFor(request asDestroyFriendship setScreenName(aScreenName))
		self
	)
	
	friendsCursor := method(screenName, TwitterFriendsCursor clone setAccount(self) setScreenName(screenName))
	followersCursor := method(screenName, TwitterFollowersCursor clone setAccount(self) setScreenName(screenName))
	
	updateStatus := method(message, tweetId,
		r := request asUpdateStatus setStatus(message) setSource(source)
		if(tweetId,
			r setInReplyToStatusId(tweetId)
		)
		resultsFor(r) at("id")// asString
	)
	
	deleteStatus := method(tweetId,
		resultsFor(request asDeleteStatus setStatusId(tweetId))
	)
	
	show := method(
		resultsFor(request asShow setScreenName(screenName))
	)
	
	showUser := method(aScreenName,
		resultsFor(request asShow setScreenName(aScreenName))
	)
	
	showUserWithId := method(anId,
		resultsFor(request asShow setUserId(anId))
	)
	
	isSuspended := method(aScreenName,
		if(aScreenName == nil, aScreenName = screenName)
		tryTwitter(showUser(aScreenName)) ifIsSuspended(
			return(true)
		) raiseUnhandled
		
		false
	)
	
	twitterIdForScreenName := method(screenName,
		self showUser(screenName) at("id") asString
	)
	
	ExceptionConditional := Object clone do(
		exception ::= nil
		result ::= nil
		done ::= false
		
		forward := method(
			//if there is an exception, check for condition
			if(exception,
				condMessageName := call message name asMutable removePrefix("if") makeFirstCharacterLowercase asSymbol
				if(exception perform(condMessageName),
					call evalArgAt(0)
					setDone(true)
				)
			)

			self
		) setPassStops(true)
		
		raiseUnhandled := method(
			if(exception,
				if(done,
					exception
				,
					exception pass
				)
			,
				result
			)
		)
		
		else := method(
			raiseUnhandled
			if(exception == nil,
				call evalArgs
			)
			result
		)
	)
	
	cursorNext := method(cursor, cursor next)
	
	userExists := method(screenName,
		tryTwitter(showUser(screenName)) ifIsSuspended(
			r := false
		) ifIsNotFound(
			r := false
		) else(
			r := true
		)
		r
	)
	
	mentions := method(
		resultsFor(request asMentions)
	)
)