Host := Object clone do(
    type := "Host"
	docCopyright("Steve Dekorte", 2004)
	docLicense("BSD revised")
	docCategory("Networking")
	
    docSlot("setName(aString)", "Set's the host name. Return self.")
    docSlot("name", "Returns name.")
    newSlot("name")

    docSlot("address", "Returns the ip address. A DNS lookup is done in the background if the address is not already known. Raises an exception on lookup  error.")

    newSlot("address")
    ip := nil

    address := method(
        if (name size == 0, Exception raise("attempt to lookup empty hostname"))
        ip = DNSResolver ipForHostName(name)
        if(ip == false, Exception raise("ip lookup for hostname '" .. name .. "' failed"))
        self address = ip
    )
)