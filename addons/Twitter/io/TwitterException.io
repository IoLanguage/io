TwitterException := Exception clone do(
	//metadoc TwitterException category Networking
	
	isBadRequest ::= false
	//doc TwitterException isBadRequest Returns isBadRequest attribute.
	
	isNotAuthorized ::= false
	//doc TwitterException isNotAuthorized Returns isNotAuthorized attribute.

	isForbidden ::= false
	//doc TwitterException isForbidden Returns isForbidden attribute.

	isNotFound ::= false
	//doc TwitterException isNotFound Returns isNotFound attribute.

	isInternalError ::= false
	//doc TwitterException isInternalError Returns isInternalError attribute.

	isDown ::= false
	//doc TwitterException isDown Returns isDown attribute.

	isOverloaded ::= false
	//doc TwitterException isOverloaded Returns isOverloaded attribute.

	isUnknown ::= false
	//doc TwitterException isUnknown Returns isUnknown attribute.

	userIsMissing ::= false
	//doc TwitterException userIsMissing Returns userIsMissing attribute.

	isAlreadyFollowing ::= false
	//doc TwitterException isAlreadyFollowing Returns isAlreadyFollowing attribute.
	
	isFollowedSelf ::= false
	//doc TwitterException isFollowedSelf Returns isFollowedSelf attribute.

	isBlocked ::= false
	//doc TwitterException isBlocked Returns isBlocked attribute.

	isSuspended ::= false
	//doc TwitterException isSuspended Returns isSuspended attribute.

	isFollowLimit ::= false
	//doc TwitterException isFollowLimit Returns isFollowLimit attribute.

	isBlockedOrSuspendedOrProtected ::= false
	//doc TwitterException isBlockedOrSuspendedOrProtected Returns isBlockedOrSuspendedOrProtected attribute.

	wasntFriend ::= false
	//doc TwitterException wasntFriend Returns wasntFriend attribute.

	isRateLimited ::= false
	//doc TwitterException isRateLimited Returns isRateLimited attribute.
	
	isStatusDuplicate ::= false
	//doc TwitterException isStatusDuplicate Returns isStatusDuplicate attribute.
	
)