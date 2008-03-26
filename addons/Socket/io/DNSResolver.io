
DNSQuery := Object clone do(
	//metadoc DNSQuery category Networking
	//metadoc DNSQuery description An object representing an individual DNS query.
	
	//doc DNSQuery hostName The host name which the query will request an IP for.
	//doc DNSQuery setHostName(aSeq) Sets the host name for the query.	
	hostName ::= nil

	init := method(
		/*doc DNSQuery coros 
		List of coroutines paused waiting for this query.
		These coros will be resumed when the query completes.
		*/	
		self coros := List clone
	)

	//doc DNSQuery addCoro(aCoro) Private method that adds a coro to coros.
	addCoro := method(coro, coros append(coro); self)
	
	//doc DNSQuery resumeCoros Resumes all the paused coros. <font color=red>Should this use resumeLater instead?</font>
	resumeCoros := method(coros foreach(resume); self) 
	
	//doc DNSQuery waitOn Pauses the calling coroutine until the query is completed.
	waitOn := method(
		coro := Scheduler currentCoroutine
		addCoro(coro)
		coro pause
		self
	)
)

DNSServer := Object clone do(
	//metadoc DNSServer category Networking
	//metadoc DNSServer description An object representing a DNSServer which DNS requests can be sent to.
	
	//doc DNSServer host The host name (usually an IP) for the DNSServer.
	//doc DNSServer setHost(aSeq) Sets the host name for the DNSServer.
	host ::= nil

	/*doc DNSServer ipForHostName(hostName, timeout)
	Sends a request to the DNS server, waits for a response and returns it. Will try 3 times if there are timeouts.
	Returns the IP (as a Sequence) if successful, raises an exception otherwise.
	*/
	ipForHostName := method(hostName, timeout,
		if(timeout == nil, timeout = 10)
		response := nil
		for(tryNumber, 1, 3,
			debugWriteln("DNSServer ", host, " request: ", hostName)
			socket := Socket clone setHost(host) ifError(continue) setPort(53) setReadTimeout(timeout)
			socket udpOpen ifError(continue)
			ipAddress := socket ipAddress
			debugWriteln("DNSServer ipForHostName(", hostName, ") sending udp query packet to ipAddress: ", ipAddress ip, " on port ", ipAddress port)
			packet := DNS dnsQueryPacketForHostName(hostName)
			debugWriteln("writing packet")
			socket udpWrite(ipAddress, packet, 0, packet size) ifError(continue)

			coroId := Scheduler currentCoroutine label
			debugWriteln(coroId, " reading DNS packet")
			socket udpReadNextChunk(ipAddress) ifError(continue)
			debugWriteln(coroId, " got DNS packet")

			socket close //Don't care about errors here
			response = DNS hostNameAndIPforDNSResponsePacket(socket readBuffer)
			response isError ifTrue(continue)
			ifNonNil(break)
			debugWriteln("trying dns resolve '", hostName, "' again")
		)
		debugWriteln(host, " response: ", response at(0), " -> ", response at(1))
		response returnIfError
		if(response,
			response at(1)
		,
			Error with("Failed to resolve " .. hostName)
		)
	)
)

DNSResolver := Object clone do(
//metadoc DNSResolver Networking
/*metadoc DNSResolver description 
With the DNSResolver you can set the DNS servers to be used for DNS lookups. Example;
<pre>
DNSResolver addDNSServerIp("128.105.2.10")
ipForYahoo := DNSResolver ipForHostName("yahoo.com")
</pre>
*/

	readTimeout ::= 10

	init := method(
		//doc DNSResolver dnsServers Returns list of DNSServer objects used for lookups.
		self dnsServers := List clone
		//doc DNSResolver queries Returns list of active DNSQuery objects.
		self queries := Map clone
		//doc DNSResolver cache Returns Map containing lookup cache.
		self cache := Map clone
	)

	init

	//doc DNSResolver emptyCache Empties the lookup cache. Returns self.
	emptyCache := method(cache empty; self)

	//doc DNSResolver addDNSServerIp(ipString) Adds a new DNSServer for the specified IP.
	addDNSServerIp := method(ip, dnsServers append(DNSServer clone setHost(ip)))

	/*doc DNSResolver setupServerListIfNeeded 
	Private method that gets local name server IPs and sets up dnsServers list. Returns self.
	*/
	setupServerListIfNeeded := method(
		if(dnsServers size > 0, return)
		DNS localNameServersIPs select(isEmpty not) foreach(ip, addDNSServerIp(ip))
		self
	)

	/*doc DNSResolver ipForHostName(hostName)
	Returns a String containing the IP for the hostName or an error if hostName could not be resolved.
	*/
	ipForHostName := method(hostName,
		setupServerListIfNeeded
		ip := cache at(hostName)
		if(ip and ip isError not, return(ip))

		q := queries at(hostName)

		if(q) then(q waitOn) else(
			q := DNSQuery clone setHostName(hostName)
			queries atPut(hostName, q)
			ip = nil

			dnsServers foreach(dnsServer,
				debugWriteln("sending query")
				//try(ip = dnsServer ipForHostName(hostName))
				ip = dnsServer ipForHostName(hostName, readTimeout)
				ip isError ifTrue(continue)
				ip ifNonNil(break)
			)
			debugWriteln("sending query done ip = ", ip)
			cache atPut(hostName, ip)
			queries removeAt(hostName)
			q resumeCoros
		)

		cache at(hostName)
	)
)
