Host := Object clone do(
	type := "Host"
	docCopyright("Steve Dekorte", 2004)
	docLicense("BSD revised")
	docCategory("Networking")

	docSlot("setName(aString)", "Set's the host name. Return self.")
	docSlot("name", "Returns name.")
	name ::= nil

	docSlot("address", "Returns the ip address. A DNS lookup is done in the background if the address is not already known. Raises an exception on lookup  error.")

	address ::= nil
	ip := nil

	address := method(
		if(name size == 0, Exception raise("attempt to lookup empty hostname"))
		DNSResolver raiseOnError(ip = DNSResolver ipForHostName(name))
		self address = ip
	)
)
