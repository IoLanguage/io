#!/usr/bin/env io
// http://developers.facebook.com/docs/reference/api/post

FBWallPost := Object clone do(
	accessToken ::= nil
	message ::= nil
	picture :: nil 
	link :: nil 
	name :: nil

	post := method(
		args := list("-F", "access_token=" .. accessToken)

		if(message, args append("-F", message))
		if(picture, args append("-F", picture))
		if(link, args append("-F", link))
		if(name, args append("-F", name))

		postCall := SystemCall clone setCommand("curl") 
		postCall setArguments(args)
		return postCall run stdout readLines
	)
)

/*
curl -F 'access_token=...' \
     -F 'message=Hello, Arjun. I like this new API.' \
     https://graph.facebook.com/arjun/feed
)
*/