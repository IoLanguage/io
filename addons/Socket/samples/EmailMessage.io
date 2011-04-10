
EmailMessage := Object clone do(
	from ::= ""
	to ::= nil
	cc ::= nil
	subject ::= nil
	content ::= nil
	
	mailServerAddress := method(
		host := to split("@") last
		address := Host clone setName(host) address
		if(address, address, host)
	)
	
	crlf := method(13 asCharacter .. 10 asCharacter)
	
	send := method(
		s := Socket clone setHost(mailServerAddress) setPort(25) 
		if (s connect == nil, return nil)
		s streamWrite("MAIL FROM:" .. from .. "\n")
		s streamWrite("RCPT TO:" .. to .. "\n")
		s streamWrite("DATA\n")
		s streamWrite("Subject: " .. subject .. "\n")
		if (cc, s streamWrite("Cc: " .. cc .. "\n"))
		s streamWrite(content .. "\n\n")
		s streamWrite(crlf .. "." .. crlf)
		s close
		self
	)
)

/*
Example use:

EmailMessage clone setTo("foo@bar.com") setFrom("up@down.com") setSubject("hi") setContent("hello world!") @send

*/