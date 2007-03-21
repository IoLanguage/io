
EmailMessage := Object clone do(
	newSlot("from", "")
	newSlot("to", nil)
	newSlot("cc", nil)
	newSlot("subject", "")
	newSlot("content", "")
	
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

