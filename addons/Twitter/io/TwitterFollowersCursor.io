TwitterFollowersCursor := TwitterFriendsFollowersCursor clone do(
	//metadoc TwitterFollowersCursor category Networking
	//metadoc TwitterFollowersCursor description Inherits from TwitterFriendsFollowersCursor. requestType is "asFollowerIds".
	setRequestType("asFollowerIds")
)