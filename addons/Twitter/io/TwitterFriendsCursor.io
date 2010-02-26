TwitterFriendsCursor := TwitterFriendsFollowersCursor clone do(
	//metadoc TwitterFriendsCursor category Networking
	//metadoc TwitterFriendsCursor description Inherits from TwitterFriendsFollowersCursor. requestType is "asFriendIds".
	setRequestType("asFriendIds")
)