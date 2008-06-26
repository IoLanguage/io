SecureClient do(
	realConnectionToServer := getSlot("connectionToServer")
	
	connectionToServer := method(host, port,
		/*
		// seemed unnecesarilly complex
		//host:port -> host:port -> host, port; 
		//host, port -> host:port -> host, port
		components := call evalArgs join(":") split(":")
		writeln("components = ", components)
		host := components at(0)
		writeln("host = ", host)
		host foreach(i,v,
			((v isDigit) or(v == ("." at(0)))) ifFalse(host = DNSResolver ipForHostName(host))
		)
		port := components at(1)
		*/
		host = DNSResolver ipForHostName(host)
		realConnectionToServer(host asString, port asString)
	)
)