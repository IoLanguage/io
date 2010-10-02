Yajl

TwitterResponse := Object clone do(
	//metadoc TwitterResponse category Networking
	body ::= nil
	//doc TwitterResponse body Returns request body.
	
	statusCode ::= nil
	//doc TwitterResponse body Returns status code.

	request ::= nil
	//doc TwitterResponse body Returns request to which this is the response.

	rateLimitRemaining ::= nil
	//doc TwitterResponse rateLimitRemaining Returns the rate limit remaining.
	
	rateLimitExpiration ::= nil
	//doc TwitterResponse rateLimitExpiration Returns the rate limit expiration.

	results ::= method(
		//doc TwitterResponse results Returns response results.
		setResults(Yajl parseJson(body))
		results
	)
	
	raiseIfError := method(
		body ifError(e,
			Exception raise(e message)
		)
		
		errorMessage := if(results type == "Map", results at("error"), "")

		statusErrorMessage := method(
			statusCode .. " " .. errorMessage
		)

		//Could not find both specified users
		if(statusCode == 400) then(
			e := TwitterException clone setIsBadRequest(true)
			if(errorMessage ?beginsWithSeq("Rate limit exceeded"),
				e setIsRateLimited(true)
			)
			e raise(if(errorMessage, statusErrorMessage, body))
		) elseif(statusCode == 401) then(
			TwitterException clone setIsNotAuthorized(true) raise(statusErrorMessage)
		) elseif(statusCode == 403) then(
			e := TwitterException clone setIsForbidden(true)
			if(errorMessage,
				if(errorMessage endsWithSeq("is already on your list.")) then(
					e setIsAlreadyFollowing(true)
				) elseif(errorMessage containsSeq("You've already requested to follow")) then(
					e setIsAlreadyFollowing(true)
				) elseif(errorMessage containsSeq("You can't follow yourself")) then(
					e setIsFollowedSelf(true)
				) elseif(errorMessage containsSeq("You have been blocked")) then(
					e setIsBlocked(true)
				) elseif(errorMessage containsSeq("You do not have permission to retrieve following status for both specified users")) then(
					e setIsBlockedOrSuspendedOrProtected(true)
				) elseif(errorMessage containsSeq("This account is currently suspended")) then(
					e setIsSuspended(true)
				) elseif(errorMessage containsSeq("User has been suspended")) then(
					e setIsSuspended(true)
				) elseif(errorMessage containsSeq("this account has been suspended")) then(
					e setIsSuspended(true)
				) elseif(errorMessage containsSeq("You are not friends with the specified user")) then(
					e setWasntFriend(true)
				) elseif(errorMessage containsSeq("You are unable to follow more people at this time")) then(
					e setIsFollowLimit(true)
				) elseif(errorMessage containsSeq("Could not find both specified users")) then(
					e setUserIsMissing(true)
				) elseif(errorMessage containsSeq("Status is a duplicate")) then(
					e setIsStatusDuplicate(true)
				)
			,
				errorMessage := body
			)
			e raise(statusErrorMessage)
		) elseif(statusCode == 404) then(
			e := TwitterException clone setIsNotFound(true)
			if(errorMessage,
				if(errorMessage containsSeq("User has been suspended")) then(
					e setIsSuspended(true)
				)
			,
				errorMessage := body
			)
			e raise(statusErrorMessage)
		) elseif(statusCode == 500) then(
			TwitterException clone setError("500 response") setIsInternalError(true) raise(body)
		) elseif(statusCode == 502) then(
			TwitterException clone setError("502 response") setIsDown(true) raise
		) elseif(statusCode == 503) then(
			TwitterException clone setError("503 response") setIsOverloaded(true) raise
		)
		
		results ifError(e,
			Exception raise(e message)
		)
		
		if(results type == "Map" and errorMessage,
			TwitterException clone raise(statusErrorMessage)
		)
		
		self
	)
)