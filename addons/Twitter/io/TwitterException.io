TwitterException := Exception clone do(
	//metadoc TwitterException category Networking
	isBadRequest ::= false
	isNotAuthorized ::= false
	isForbidden ::= false
	isNotFound ::= false
	isInternalError ::= false
	isDown ::= false
	isOverloaded ::= false
	isUnknown ::= false
	userIsMissing ::= false
	isAlreadyFollowing ::= false
	isBlocked ::= false
	isSuspended ::= false
	isFollowLimit ::= false
	isBlockedOrSuspendedOrProtected ::= false
	wasntFriend ::= false
	isRateLimited ::= false
)