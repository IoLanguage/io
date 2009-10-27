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
			TwitterException clone setIsRateLimited(true) raise
		)
		request execute
		
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
		show(aScreenName) at("protected")
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
		resultsFor(request asCreateFriendship setScreenName(aScreenName))
		self
	)
	
	unfollow := method(aScreenName,
		//You are not friends with the specified user
		
		resultsFor(request asDestroyFriendship setScreenName(aScreenName))
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
	
	showUser := method(aScreenName,
		resultsFor(request asShow setScreenName(aScreenName))
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
						return(ExceptionConditional clone setException(e))
					)
				,
					e pass
				)
			,
				return(ExceptionConditional clone setResult(result) setDone(true))
			)
		)
		
		e pass
	)
)