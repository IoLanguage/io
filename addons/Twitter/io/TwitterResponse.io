Yajl

TwitterResponse := Object clone do(
	//api
	body ::= nil
	statusCode ::= nil
	request ::= nil
	rateLimitRemaining ::= nil
	rateLimitExpiration ::= nil

	results ::= method(
		setResults(Yajl parseJson(body))
		results
	)
	
	raiseIfError := method(
		body ifError(e,
			Exception raise(e message)
		)
		
		//Could not find both specified users
		if(statusCode == 400) then(
			TwitterException clone setIsBadRequest(true) raise(body)
		) elseif(statusCode == 401) then(
			TwitterException clone setIsNotAuthorized(true) raise
		) elseif(statusCode == 403) then(
			e := TwitterException clone setIsForbidden(true)
			if(errorMessage := results ?at("error"),
				if(errorMessage endsWithSeq("is already on your list.")) then(
					e setIsAlreadyFollowing(true)
				) elseif(errorMessage containsSeq("You have been blocked")) then(
					e setIsBlocked(true)
				) elseif(errorMessage containsSeq("You do not have permission to retrieve following status for both specified users")) then(
					e setIsBlockedOrSuspendedOrProtected(true)
				) elseif(errorMessage containsSeq("This account is currently suspended")) then(
					e setIsSuspended(true)
				) elseif(errorMessage containsSeq("You are not friends with the specified user")) then(
					e setWasntFriend(true)
				) elseif(errorMessage containsSeq("You are unable to follow more people at this time")) then(
					e setIsFollowLimit(true)
				) elseif(errorMessage containsSeq("Could not find both specified users")) then(
					e setUserIsMissing(true)
				)
			,
				errorMessage := body
			)
			e raise(errorMessage)
		) elseif(statusCode == 404) then(
			e := TwitterException clone setIsNotFound(true)
			if(errorMessage := results ?at("error"),
				if(errorMessage containsSeq("User has been suspended")) then(
					e setIsSuspended(true)
				)
			,
				errorMessage := body
			)
			e raise(errorMessage)
		) elseif(statusCode == 500) then(
			TwitterException clone setIsInternalError(true) raise(body)
		) elseif(statusCode == 502) then(
			TwitterException clone setIsDown(true) raise
		) elseif(statusCode == 503) then(
			TwitterException clone setIsOverloaded(true) raise
		)
		
		results ifError(
			Exception raise(e message)
		)
		
		if(errorMessage := results ?at("error"),
			TwitterException clone  raise(errorMessage)
		)
		
		self
	)
)