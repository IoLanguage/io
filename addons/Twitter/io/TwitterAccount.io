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
	
	followerIdsCursor ::= "-1"
	resetFollowerIdsCursor := method(setFollowerIdsCursor("-1"))
	hasMoreFollowerIds := method(followerIds != "0")
	followerIds := method(aScreenName,
		result := resultsFor(request asFollowerIds setScreenName(aScreenName) setCursor(followerIdsCursor))
		setFollowerIdsCursor(result at("next_cursor") asString)
		writeln(followerIdsCursor)
		result at("ids")
	)
	
	friendIdsCursor ::= "-1"
	resetFriendIdsCursor := method(setFriendIdsCursor("-1"))
	hasMoreFriendIds := method(followerIds != "0")
	friendIds := method(aScreenName,
		result := resultsFor(request asFriendIds setScreenName(aScreenName) setCursor(friendIdsCursor))
		setFriendIdsCursor(result at("next_cursor") asString)
		writeln(friendIdsCursor)
		result at("ids")
	)
	
	updateStatus := method(message,
		resultsFor(request asUpdateStatus setStatus(message) setSource(source)) at("id")// asString
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
		handleErrors(showUser(aScreenName)) ifIsSuspended(
			return(true)
		) else(
			return(false)
		)
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
		
		else := method(
			if(exception,
				if(done,
					//exception was handled
					self
				,
					//exception wasn't handled
					exception pass
				)
			,
				//no exception
				if(call message arguments size == 2,
					call sender setSlot(call message arguments at(0) name, result)
					messageArg := 1
				,
					messageArg := 0
				)
				
				call evalArgAt(messageArg)
				self
			)
		) setPassStops(true)
	)
	
	handleErrors := method(
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