
v := Object clone do(
	from ::= ""
	to ::= nil
	cc ::= nil
	subject ::= ""
	content ::= ""
	
	mailServerAddress := method(
		host := to split("@") last
		Host clone setName(host) mailServerAddresses first
	)
	
	crlf := method(13 asCharacter .. 10 asCharacter)
	
	send := method(
		s := Socket clone setHost(mailServerAddress) setPort(25) 
		if (s connect == nil, return nil)
		s writeln("MAIL FROM:", from)
		s writeln("RCPT TO:", to)
		s writeln("DATA")
		s writeln("Subject: ", subject)
		if (cc, s write("Cc: ", cc))
		s writeln(content)
		s write(crlf, ".", crlf)
		s close
		self
	)
)

server := Server clone do(
	setPort(8181)
	handleSocket := method(socket,
		socket println
		while(socket isOpen, socket read)
		m := doString(socket readBuffer)
		EmailMessage clone setTo(m to) setFrom(m from) setCC(m cc) setSubject(m subject) setContent(m content) @send
	)
)

server start
