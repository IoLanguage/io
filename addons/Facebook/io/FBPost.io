#!/usr/bin/env io

/*

docs at 
http://developers.facebook.com/docs/reference/api/post


*/

FBPost := Object clone do(
	accessToken ::= nil
	message ::= nil
	picture ::= nil 
	link ::= nil 
	name ::= nil
	caption ::= nil
	description ::= nil
	url ::= nil 
	
	post := method(
		args := list("-F", "access_token=" .. accessToken)

		if(message,     args append("-F", "message=" .. message))
		if(picture,     args append("-F", "picture=" .. picture))
		if(link,        args append("-F", "link=" .. link))
		if(caption,     args append("-F", "caption=" .. caption))
		if(name,        args append("-F", "name=" .. name))
		if(description, args append("-F", "description=" .. name))
		args append(url)

		postCall := SystemCall clone setCommand("curl") 
		postCall setArguments(args)
		return postCall run stdout readLines
	)
)

