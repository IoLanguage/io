#!/usr/bin/env io

/*
docs at 
http://developers.facebook.com/docs/reference/api/post
*/

FBPost := Object clone do(
	accessToken ::= nil
	messageText ::= nil
	picture ::= nil 
	link ::= nil 
	name ::= nil
	caption ::= nil
	description ::= nil
	url ::= nil 
	error ::= nil
	postId ::= nil
	debug ::= false
	
	post := method(
		args := list("-F", "access_token=" .. accessToken)

		if(messageText, args append("-F", "message=" .. messageText))
		if(picture,     args append("-F", "picture=" .. picture))
		if(link,        args append("-F", "link=" .. link))
		if(caption,     args append("-F", "caption=" .. caption))
		if(name,        args append("-F", "name=" .. name))
		if(description, args append("-F", "description=" .. description))
		args append(url)

		postCall := SystemCall clone setCommand("curl") 
		postCall setArguments(args)
		
		if(debug,
			writeln("")
			write("curl ")
			args foreach(arg, write("'", arg, "' "))
			writeln("")
		)
		
		postCall run 
		
		/*
		err := postCall stderr readLines
		if(err first, 
			setError(err join("\n")); 
			writeln("error = ", error)
			return nil
		)
		*/
		
		out := postCall stdout readLines
		outJson := Yajl clone parse(out first) root first
		setPostId(outJson at("id"))
		return self
	)
)


