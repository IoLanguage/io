URL

Sequence asCamelized := method(
	split("_") map(i, v, if(i > 0, v asCapitalized, v)) join
)

TwitterRequest := Object clone do(
	//metadoc TwitterRequest category Networking
	//metadoc TwitterRequest description Represents a Twitter API request and contains its results.
	
	host ::= "api.twitter.com"
	//doc TwitterRequest host "api.twitter.com"

	account ::= nil
	//doc TwitterRequest account Returns the account to pull the Oauth tokens from
	
	httpMethod ::= "get"
	//doc TwitterRequest httpMethod "get"
	
	path ::= nil
	//doc TwitterRequest path Request path.
	
	queryParamNames ::= nil
	postParamNames ::= nil
	pathParamNames ::= nil
	fileParamName ::= nil
	
	delegate ::= nil
	//doc TwitterRequest delegate Delegate for request callbacks.
	
	init := method(
		setQueryParamNames(List clone)
		setPostParamNames(List clone)
		setPathParamNames(List clone)
	)
	
	response ::= nil
	//doc TwitterRequest response This slot holds a TwitterResponse object for this request.
	
	//public
	
	execute := method(
		//doc TwitterRequest execute Sends the request. Returns the response object.
		queryParams := Map clone
		queryParamNames foreach(name,
			if(v := self perform(name asCamelized asSymbol),
				queryParams atPut(name, v asString)
			)
		)
		queryString := queryParams asQueryString
		
		postParams := Map clone
		postParamNames foreach(name,
			if(v := self perform(name asCamelized asSymbol),
				postParams atPut(name, v asString)
			)
		)
		
		pathParamNames foreach(name,
			if(v := self perform(name asCamelized asSymbol),
				path = path appendPathSeq(v asString)
			)
		)
		
		setHttpMethod(httpMethod asUppercase)
		
		req := OauthRequest clone
		//req setUrl("http://" .. host .. path .. ".json" .. queryString)
		req setUrlPath("http://" .. host .. "/1" .. path .. ".json") setQueryParams(queryParams)
		
		debugWriteln(req url)
		
		///*
		req setCallback(nil)
		req setConsumerKey(account consumerKey)
		req setConsumerSecret(account consumerSecret)
		req setToken(account token)
		req setTokenSecret(account tokenSecret)
		//*/
		
		/*
		req setCallback(nil)
		req setConsumerKey("GDdmIQH6jhtmLUypg82g")
		req setNonce("oElnnMTQIZvqvlfXM56aBLAf5noGD0AQR3Fmi7Q6Y")
		req setToken("819797-Jxq8aYUDRmykzVKrgoLhXSq67TEa5ruc4GJC2rWimw")
		req setTimestamp("1272325550")
		
		req setConsumerSecret("MCD8BKwGdgPHvAuvgvz4EQpqDAtx89grbuNMRd7Eh98")
		req setTokenSecret("J6zix3FfA9LofH0awS24M3HcBYXO5nI1iYe8EfBA")
		*/
		
		resp := if(httpMethod == "GET",
			req get
		,
			req setPostParams(postParams)
			req post
		)

		response := TwitterResponse clone
		response setBody(resp body)
		response setStatusCode(resp statusCode)
		response setRateLimitRemaining(resp headers at("X-RateLimit-Remaining"))
		response setRateLimitExpiration(resp headers at("X-RateLimit-Reset"))
		
		setResponse(response)
		
		debugWriteln("TwitterResponse body[", response body, "]")
		debugWriteln("TwitterResponse statusCode[", response statusCode, "]")
		response
	)
	
	addQuerySlots := method(querySlotNames,
		querySlotNames split foreach(name,
			self newSlot(name asCamelized)
			queryParamNames appendIfAbsent(name)
		)
		self
	)
	
	addPostSlots := method(postSlotNames,
		postSlotNames split foreach(name,
			self newSlot(name asCamelized)
			postParamNames appendIfAbsent(name)
		)
		self
	)
	
	addPathSlots := method(slotNames,
		slotNames split foreach(name,
			self newSlot(name asCamelized)
			pathParamNames appendIfAbsent(name)
		)
		self
	)
	
	addFileSlot := method(slotName,
		self newSlot(slotName asCamelized)
		setFileParamName(slotName)
		self
	)
	
	dontAuthenticate := method(
		setUsername(nil) setPassword(nil)
	)
	
	asShowFriendship := method(
		setPath("/friendships/show")
		addQuerySlots("source_id source_screen_name target_id target_screen_name")
	)
	
	asFriendshipExists := method(
		setPath("/friendships/exists")
		addQuerySlots("user_a user_b")
	)
	
	asShow := method(
		setPath("/users/show")
		addQuerySlots("id user_id screen_name")
	)
	
	asCreateFriendship := method(
		setHttpMethod("post")
		setPath("/friendships/create")
		//addQuerySlots("user_id screen_name follow")
		addPostSlots("user_id screen_name follow")
	)
	
	asDestroyFriendship := method(
		self\
		setHttpMethod("post")\
		setPath("/friendships/destroy")\
		//addQuerySlots("user_id screen_name")
		addPostSlots("user_id screen_name")
	)
	
	asFriendIds := method(
		setPath("/friends/ids")
		addQuerySlots("user_id screen_name cursor")
	)
	
	asFollowerIds := method(
		setPath("/followers/ids")
		addQuerySlots("user_id screen_name cursor")
	)
	
	asUpdateAccountProfile := method(
		setHttpMethod("post")
		setPath("/account/update_profile")
		addPostSlots("name url location description")
	)
	
	asUpdateAccountProfileColors := method(
		setHttpMethod("post")
		setPath("/account/update_profile_colors")
		addPostSlots("profile_background_color profile_text_color profile_link_color profile_sidebar_fill_color profile_sidebar_border_color")
	)
	
	asUpdateAccountProfileImage := method(
		setHttpMethod("post")
		setPath("/account/update_profile_image")
		addFileSlot("image")
	)
	
	asUpdateAccountProfileBackgroundImage := method(
		setHttpMethod("post")
		setPath("/account/update_profile_background_image")
		addFileSlot("image")
		addQuerySlots("tile")
	)
	
	asUpdateStatus := method(
		setHttpMethod("post")
		setPath("/statuses/update")
		//addQuerySlots("source in_reply_to_status_id")
		addPostSlots("status source in_reply_to_status_id")
	)
	
	asDeleteStatus := method(
		setHttpMethod("post")
		self path := method(
			"/statuses/destroy/#{statusId}" interpolate
		)
		self newSlot("statusId")
		self
	)
	
	asSearch := method(
		setHost("search.twitter.com")
		setPath("/search")
		addQuerySlots("rpp page since_id q")
	)
	
	asRateLimitStatus := method(
		setPath("/account/rate_limit_status")
	)
	
	asMentions := method(
		setPath("/statuses/mentions")
	)
	
	asRetweet := method(
		setHttpMethod("post")
		self path := method(
			"/statuses/retweet/#{tweetId}" interpolate
		)
		self newSlot("tweetId")
		self
	)
	
	asPublicTimeline := method(
		setPath("/statuses/public_timeline")
	)
	
	asHomeTimeline := method(
		setPath("/statuses/home_timeline")
	)
	
	asUserTimeline := method(
		setPath("/statuses/user_timeline")
	)
)