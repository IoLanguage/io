TwitterAccount := Object clone do(
	//metadoc TwitterAccount category Networking
/*metadoc TwitterAccount description 
Object representing a twitter account.	

*/
	screenName ::= nil
	//doc TwitterAccount screenName Returns the account screenName.
	//doc TwitterAccount setScreenName(aSeq) Sets the account screenName. Returns self.
	
	password ::= nil
	//doc TwitterAccount password Returns the account password.
	//doc TwitterAccount setPassword(aSeq) Sets the account password. Returns self.
	
	consumerKey ::= nil
	//doc TwitterAccount consumerKey Returns the OAuth consumerKey.
	//doc TwitterAccount setConsumerKey(aKey) Sets the OAuth consumerKey.  Returns self.
	
	consumerSecret ::= nil
	//doc TwitterAccount consumerSecret Returns the OAuth consumerSecret.
	//doc TwitterAccount setConsumerSecret(aKey) Sets the OAuth consumerSecret.  Returns self.
	
	token ::= nil
	//doc TwitterAccount token Returns the OAuth token.
	//doc TwitterAccount setToken(aToken) Sets the OAuth token.  Returns self.
	
	tokenSecret ::= nil
	//doc TwitterAccount accessTokenSecret Returns the OAuth token secret.
	//doc TwitterAccount setAccessTokenSecret(aTokenSecret) Sets the OAuth token secret.  Returns self.
		
	profile ::= nil
	//doc TwitterAccount profile Returns the account Profile object.
	//doc TwitterAccount setProfile(aProfile) Sets the account profile. Returns self.
		
	rateLimitRemaining ::= nil
	//doc TwitterAccount rateLimitRemaining Returns the account rateLimitRemaining.
	//doc TwitterAccount setRateLimitRemaining(aNumber) Sets the account rateLimitRemaining. Returns self.
	
	rateLimitExpiration ::= nil
	//doc TwitterAccount rateLimitExpiration Returns the account rateLimitExpiration.
	//doc TwitterAccount setRateLimitExpiration(aNumber) Sets the account rateLimitExpiration. Returns self.	

	init := method(
		setProfile(TwitterAccountProfile clone setAccount(self))
	)
	
	isLimited := method(
		//doc TwitterAccount isLimited Returns true if the account's rate limit is exceeded, false otherwise.
		if(rateLimitRemaining == nil,
			updateRateLimits
		)
		rateLimitRemaining == 0
	)
	
	request := method(
		//doc TwitterAccount request Returns a new TwitterRequest object for this account.
		TwitterRequest clone setAccount(self)
	)
	
	resultsFor := method(request,
		//doc TwitterAccount resultsFor(aRequest) Returns results for the request.
		//if(isLimited,
		//	TwitterException clone setIsRateLimited(true) raise("Rate Limited")
		//)
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
		//doc TwitterAccount updateRateLimits Updates the rate limits. Returns self.
		
		r := request asRateLimitStatus execute raiseIfError results
		
		setRateLimitRemaining(r at("remaining_hits") asNumber)
		setRateLimitExpiration(Date clone fromNumber(r at("reset_time_in_seconds") asNumber))
		self
	)
	
	hasFriend := method(aScreenName,
		//doc TwitterAccount hasFriend(aScreenName) Returns true if the account has the specified friend, false otherwise.
		//Could not find target user.
		resultsFor(request asShowFriendship setTargetScreenName(aScreenName)) at("relationship") at("source") at("following")
	)
	
	hasFollower := method(aScreenName,
		//doc TwitterAccount hasFollower(aScreenName) Returns true if the account has the specified follower, false otherwise.
		//Could not find target user.
		resultsFor(request asFriendshipExists setUserA(aScreenName) setUserB(screenName))
	)
	
	hasProtectedUpdates := method(aScreenName,
		//doc TwitterAccount hasProtectedUpdates Returns true if the account has protected updates, false otherwise.
		showUser(aScreenName) at("protected")
	)
	
	/* for testing
	raiseFollowException := method(
		//self raiseFollowException := nil
		TwitterException clone setIsFollowLimit(true) raise("Follow limit reached")
	)
	*/
	
	follow := method(aScreenName,
		//doc TwitterAccount follow(aScreenName) Follow the user with the specified screen name. Returns results of the request.	
		//Could not follow user: richcollins is already on your list.
		//Could not follow user: You have been blocked from following this account at the request of the user.
		//Could not follow user: This account is currently suspended and is being investigated due to strange activity
		//raiseFollowException for testing
		resultsFor(request asCreateFriendship setScreenName(aScreenName)) at("protected")
	)
	
	followId := method(userId,
		//doc TwitterAccount followId(userId) Follow the user with the specified id. Returns results of the request.
		resultsFor(request asCreateFriendship setUserId(userId))
	)
	
	unfollow := method(aScreenName,
		//doc TwitterAccount unfollow(aScreenName) Unfollow the user with the specified screen name. Returns self.
		//You are not friends with the specified user
		
		resultsFor(request asDestroyFriendship setScreenName(aScreenName))
		self
	)
	
	unfollowId := method(userId,
		//doc TwitterAccount unfollowId(userId) Unfollow the user with the specified screen id. Returns self.
		
		resultsFor(request asDestroyFriendship setUserId(userId))
		self
	)
	
	friendsCursor := method(screenName, 
		//doc TwitterAccount friendsCursor Returns a new TwitterFriendsCursor instance for this account.
		TwitterFriendsCursor clone setAccount(self) setScreenName(screenName)
	)
	
	followersCursor := method(screenName, 
		//doc TwitterAccount followersCursor Returns a new TwitterFollowersCursor instance for this account.		
		TwitterFollowersCursor clone setAccount(self) setScreenName(screenName)
	)
	
	updateStatus := method(message, tweetId,
		//doc TwitterAccount updateStatus(messageText, tweetId) Updates the status message and returns the results of the request.		
		r := request asUpdateStatus setStatus(message)
		if(tweetId,
			r setInReplyToStatusId(tweetId)
		)
		resultsFor(r) at("id")// asString
	)
	
	deleteStatus := method(tweetId,
		//doc TwitterAccount deleteStatus(tweetId) Deletes the specified tweet and returns the results of the request.		
		resultsFor(request asDeleteStatus setStatusId(tweetId))
	)
	
	show := method(
		//doc TwitterAccount show Returns a Map containing attributes related to the Twitter user associated with this account
		resultsFor(request asShow setScreenName(screenName))
	)
	
	showUser := method(aScreenName,
		//doc TwitterAccount showUser(aScreenName) Returns a Map containing attributes related to the Twitter user associated with aScreenName
		resultsFor(request asShow setScreenName(aScreenName))
	)
	
	showUserWithId := method(anId,
		//doc TwitterAccount showUserWithId(anId) Returns a Map containing attributes related to the Twitter user associated with anId
		resultsFor(request asShow setUserId(anId))
	)
	
	isSuspended := method(aScreenName,
		//doc TwitterAccount isSuspended(aScreenName) Returns true if the specified screenName is a suspended account, false otherwise.	
		if(aScreenName == nil, aScreenName = screenName)
		tryTwitter(showUser(aScreenName)) ifIsSuspended(
			return(true)
		) raiseUnhandled
		
		false
	)
	
	twitterIdForScreenName := method(screenName,
		//doc TwitterAccount twitterIdForScreenName(aScreenName) Returns twitter id for the specified screenName.	
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
	
	cursorNext := method(cursor, 
		cursor next
	)
	
	
	userExists := method(screenName,
		//doc TwitterAccount userExists(aScreenName) Returns true if the specified user exists, false otherwise.	
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
		//doc TwitterAccount mentions Returns mentions for this account.	
		resultsFor(request asMentions)
	)
	
	retweet := method(tweetId,
		//doc TwitterAccount retweet(tweetId) Retweets the tweet with tweetId
		r := request asRetweet setTweetId(tweetId)
		resultsFor(r) at("id") asString
	)
	
	requestOAuthAccess := method(
		/*doc TwitterAccount requestOAuthAccess 
		Sets the token and tokenSecret using CURL + Oauth oob.
		consumerKey, consumerSecret, username and password must be set.  Returns self
		*/

		TwitterOauthSession clone setAccount(self) requestOauthAccess account
	)
	
	publicTimeline := method(
		//doc TwitterAccount publicTimeline Returns the public timeline tweets for this account
		resultsFor(request asPublicTimeline)
	)
	
	homeTimeline := method(
		//doc TwitterAccount homeTimeline Returns the home timeline tweets for this account
		resultsFor(request asHomeTimeline)
	)
	
	userTimeline := method(
		//doc TwitterAccount userTimeline Returns the user timeline tweets for this account
		resultsFor(request asUserTimeline)
	)
)