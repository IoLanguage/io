Host := Object clone do(
	//metadoc Host copyright Steve Dekorte, 2004
	//metadoc Host license BSD revised
	//metadoc Host category Networking

	//doc Host setName(aString) Set's the host name. Return self.
	//doc Host name Returns name.
	name ::= nil

	/*doc Host address 
	Returns the IP address. A DNS lookup is done in the background if the 
	address is not already known. Returns an error on lookup failure.
	*/
	
	address ::= nil
	ip := nil

	address := method(
		if(name size == 0, return(Error with("Attempted to lookup empty hostname")))
		if((ip = DNSResolver ipForHostName(name)) isError not,
			self address = ip
		,
			Error with("Could not resolve " .. name)
		)
	)
)
