SecureClient do(
	realConnectionToServer := getSlot("connectionToServer")
	connectionToServer := method(
		//host:port -> host:port -> host, port; host, port -> host:port -> host, port
		components := call evalArgs join(":") split(":")
		host := components at(0)
		host foreach(i,v,
			((v isDigit) or(v == ("." at(0)))) ifFalse(host = DNSResolver ipForHostName(host))
		)
		port := components at(1)
		realConnectionToServer(host asString, port asString)
	)
)