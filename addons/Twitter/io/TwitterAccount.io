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
	
	accessToken ::= nil
	//doc TwitterAccount accessToken Returns the OAuth access token.
	//doc TwitterAccount setAccessToken(aToken) Sets the OAuth access token.  Returns self.
	
	accessTokenSecret ::= nil
	//doc TwitterAccount accessTokenSecret Returns the OAuth access token secret.
	//doc TwitterAccount setAccessTokenSecret(aTokenSecret) Sets the OAuth access token secret.  Returns self.
		
	profile ::= nil
	//doc TwitterAccount profile Returns the account Profile object.
	//doc TwitterAccount setProfile(aProfile) Sets the account profile. Returns self.
		
	source ::= "API"
	//doc TwitterAccount source Returns the account source (e.g. "API").
	//doc TwitterAccount setSource(aSource) Sets the account source. Returns self.
		
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
		r := request asUpdateStatus setStatus(message) setSource(source)
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
	
	OAuthSession := Object clone do(
		MD5
		
		/*
		req := Oauth request
        req setUrl(someUrl)
        req setConsumerKey(account consumerKey)
        req setConsumerSecret(account consumerSecret)
        req setAccessToken(account accessToken)
        req setAccessTokenSecret(account accessTokenSecret)
        req get
        req setPostParams(postParams)
        resp := req post
        
        resp statusCode
        resp body
        
        */

        OauthRequest := Object clone do(
            url ::= nil
            headers ::= nil
            params ::= nil
            body ::= nil
            
            consumerSecret ::= nil
            
            init := method(
                self headers := Map clone
                self params := Map clone
            )
            
            oauthNonce := method(
                (Date clone now asNumber asString .. "stylous") md5String
            )
		
            oauthTimestamp := method(
			     Date clone now asNumber asString beforeSeq(".")
            )
            
            post := method(
                params atPut("oauth_nonce", oauthNonce)
                params atPut("oauth_signature_method", "HMAC-SHA1")
                params atPut("oauth_timestamp", oauthTimestamp)
                params atPut("oauth_version", "1.0")
                calcAuthorizationHeader
                sc := SystemCall clone 
                sc setCommand("curl") 
                args := list(url)
                headers foreach(k, v, args append("--header", "" .. k .. ": " .. v .. ""))
                if(body, args append("--data", body))
                args append("-d", "")
                writeln("\ncurl\n  ", args join("\n  "), "\n")
                sc setArguments(args)
                sc run 
                data := sc stdout readLines join("\n")
                if(data containsSeq("=") not, return data) //Exception raise("OauthRequest error: ", data))
                
                writeln(url, "\n  [", data split("&"), "]\n\n")
                outMap := Map clone
                data split("&") foreach(kv, 
                    kv = kv split("=")
                    //writeln("kv: '", kv first, "' : '", kv second, "'")
                    outMap atPut(kv first, kv second)
                )
                writeln("-------------------------")
                return outMap
            )
            
            Map asOAuthBaseSeq := method(
                keys sort map(k,
                    k urlEncoded .. "%3D" .. at(k) urlEncoded
                ) join("%26")
            )
            Map asOAuthHeader := method(
                "OAuth " .. keys sort map(k,
                    k urlEncoded .. "=\"" .. at(k) urlEncoded .. "\""
                ) join(", ")
            )
            
            calcAuthorizationHeader := method(
                baseSeq := list("POST", url urlEncoded, params asOAuthBaseSeq) join("&")
                authHeader := params clone atPut("oauth_signature", SHA1 hmac(signingKey, baseSeq) asBase64 removeLast) asOAuthHeader
                self headers atPut("Authorization", authHeader)            
            )
            
            signingKey := method(consumerSecret .. "&")	

        )
	
		account ::= nil
		pin ::= nil
		
		requestOAuthAccess := method(
			requestToken
			requestAccess
			requestAccessToken
		)
		
		
		oauthCallback ::= "oob"
		//oauthCallback ::= "http://localhost:3005/the_dance/process_callback?service_provider_id=11"	
		
		oauthToken ::= nil
		oauthSecret ::= nil
		
		requestToken := method(			
			p := Map clone
			p atPut("oauth_callback", oauthCallback)
			p atPut("oauth_consumer_key", account consumerKey)
			r := OauthRequest clone setUrl("https://api.twitter.com/oauth/request_token") setConsumerSecret(account consumerSecret) setParams(p) post
			writeln("oauth_token= ", r at("oauth_token"))
			writeln("oauth_token_secret= ", r at("oauth_token_secret"))
            setOauthToken(r at("oauth_token"))
            setOauthSecret(r at("oauth_token_secret"))
            self
		)
		
		requestPin := method(
            SGML
            // load user auth page
            url := "http://api.twitter.com/oauth/authorize?oauth_token=" .. oauthToken
            writeln("curl ", url, " "); File standardOutput flush
            form := URL with(url) fetch asSGML
            
            //sc := SystemCall clone setCommand("curl") setArguments(list(url))
            //sc run 
            //form := sc stdout readLines join("\n") asSGML

            
            params := Map clone
            form elementsWithName("input") foreach(e,
                k := e attributes at("name")
                v := e attributes at("value")
                if(k and v, params atPut(k, v))
            )
            
            // input user/pass into auth page form
            params atPut("session[username_or_email]", account screenName)
            params atPut("session[password]", account password)
            params removeAt("cancel")
            
            //params foreach(k, v, writeln(k, ":", v))
            
            // load pin request page
            pinPage := URL with("http://api.twitter.com/oauth/authorize") post(params) 
            daPin := pinPage asSGML elementsWithNameAndId("div", "oauth_pin") first allText
            self setPin(daPin asMutable strip)
            writeln("got pin: ", pin)
            writeln("------------------")
            self
        )


        accessKey ::= nil
        accessSecret ::= nil
        
		requestAccess := method(
            p := Map clone
            p atPut("oauth_consumer_key", account consumerKey)
            p atPut("oauth_token", oauthToken)
            p atPut("oauth_verifier", pin)            
            r := OauthRequest clone setUrl("https://api.twitter.com/oauth/access_token") setConsumerSecret(account consumerSecret) setParams(p) post
            setAccessKey(r at("oauth_token"))
            setAccessSecret(r at("oauth_token_secret"))
            writeln("got accessKey: ", accessKey)
            writeln("got accessSecret: ", accessSecret)           
            writeln("------------------")
            self
		)
		
		requestUrl := method(url, body,
            p := Map clone
            p atPut("oauth_consumer_key", account consumerKey)
            p atPut("oauth_token", accessKey)
            r := OauthRequest clone setUrl(url) setConsumerSecret(accessSecret) setParams(p) setBody(body) post          
		)

	)
	
	requestOAuthAccess := method(
		/*doc TwitterAccount requestOAuthAccess 
		Sets the accessToken and accessTokenSecret using CURL + Twitter oob pin.  
		consumerKey, consumerSecret, username and password must be set.  Returns self
		*/
		OAuthSession clone setAccount(self) requestToken requestPin requestAccess requestUrl("https://api.twitter.com/1/statuses/update.json", "status=hello+world")
	)
)