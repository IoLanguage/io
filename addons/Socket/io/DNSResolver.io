
DNSQuery := Object clone do(
	docCategory("Networking")
	newSlot("hostName")
	init := method(self coros := List clone)
	addCoro := method(coro, coros append(coro); self)
	resumeCoros := method(coros foreach(resume); self)
	waitOn := method(
		coro := Scheduler currentCoroutine
		addCoro(coro)
		coro pause
		self
	)
)

DNSServer := Object clone do(
	docCategory("Networking")
	newSlot("host", nil)
	//debugWriteln := getSlot("writeln")
	debugWriteln := nil

	/*
	hostNameIsIP := method(hostName,
		if(hostName occurancesOfSeq(".") == 3,
			hostName foreach(c, if(c isLetter, return false))
			return true
		)
		false
	)
	*/

	ipForHostName := method(hostName, timeout,
		if(timeout == nil, timeout = 10)
		//if(self hostNameIsIP(hostName), return hostName)
		for(tryNumber, 1, 3,
			debugWriteln("DNSServer ", host, " request: ", hostName)
			socket := Socket clone setHost(host) setPort(53) setReadTimeout(timeout)
			socket udpOpen
			ipAddress := socket ipAddress
			debugWriteln("DNSServer ipForHostName(", hostName, ") sending udp query packet to ipAddress: ", ipAddress ip, " on port ", ipAddress port)
			packet := DNS dnsQueryPacketForHostName(hostName)
			debugWriteln("writing packet")
			socket udpWrite(ipAddress, packet, 0, packet size)

			coroId := Scheduler currentCoroutine label
			debugWriteln(coroId, " reading DNS packet")
			socket udpReadNextChunk(ipAddress)
			debugWriteln(coroId, " got DNS packet")

			socket close
			e := try(
				list := DNS hostNameAndIPforDNSResponsePacket(socket readBuffer)
			)
			if(e == nil, break)
			//e coroutine backTraceString println
			writeln("trying dns resolve '", hostName, "' again")
		)
		debugWriteln(host, " response: ", list at(0), " -> ", list at(1))
		list at(1)
	)
)

DNSResolver := Object clone do(
	docCategory("Networking")
	docDescription("""With the DNSResolver you can set the DNS servers to be used for DNS lookups. Example;<pre>
  DNSResolver addDNSServerIp("128.105.2.10")
  ipForYahoo := DNSResolver ipForHostName("yahoo.com")
</pre>
""")

	init := method(
		self dnsServers := List clone
		self queries := Map clone
		self cache := Map clone
	)

	init

	//debugWriteln := getSlot("writeln")
	debugWriteln := nil

	docSlot("dnsServers", "Returns list of DNSServer objects used for lookups.")
	docSlot("queries", "Returns list of active DNSQuery objects.")
	docSlot("cache", "Returns Map containing lookup cache.")
	docSlot("emptyCache", "Empties the lookup cache. Returns self.")

	emptyCache := method(cache empty; self)

	docSlot("addDNSServerIp(ipString)", "Adds a new DNSServer for the specified IP.")
	addDNSServerIp := method(ip, dnsServers append(DNSServer clone setHost(ip)))

	setupServerListIfNeeded := method(
		if(dnsServers size > 0, return)
		DNS localNameServersIPs foreach(ip, addDNSServerIp(ip))
	)

	docSlot("ipForHostName(hostName)", "Returns a String containing the IP for the hostName or 'failed', if an error occurs.")

	ipForHostName := method(hostName,
		setupServerListIfNeeded
		ip := cache at(hostName)
		if(ip, return ip)

		q := queries at(hostName)

		if(q) then(q waitOn) else(
			q := DNSQuery clone setHostName(hostName)
			queries atPut(hostName, q)
			ip := nil

			dnsServers foreach(dnsServer,
				debugWriteln("sending query")
				//try(ip = dnsServer ipForHostName(hostName))
				ip = dnsServer ipForHostName(hostName)
				if(ip, break)
			)
			debugWriteln("sending query done ip = ", ip)

			ip ifNil(ip = false)
			cache atPut(hostName, ip)
			queries removeAt(hostName)
			q resumeCoros
		)

		cache at(hostName)
	)
)
