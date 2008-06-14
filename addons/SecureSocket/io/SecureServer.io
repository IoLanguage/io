SecureServer do(
    debugWriteln := nil
    debugOff := method(self debugWriteln := nil)
    debugOn := method(self debugWriteln := getSlot("writeln"))
	//debugOn

	newSlot("coro", nil)
	newSlot("dispatchCoro", nil)
	
	rawUseTLS := getSlot("useTLS")
	rawUseDTLS := getSlot("useDTLS")
	usesTLS := true
	useTLS := method(
		rawUseTLS
		usesTLS = true
		self
	)
	useDTLS := method(
		rawUseDTLS
		usesTLS = false
		self
	)

	init := method(
	   self socket := Socket clone setPort(4433) setReadTimeout(356*24*60*60)
	   self
	)
	
	//doc SecureServer setHost(hostName) Sets the hostName. Returns self.")
	
	setHost := method(host, socket setHost(host); self)
	
	//doc SecureServer setPort(aNumber) Sets the port on which the server will listen  for connections. Returns self.")
	
	setPort := method(port, socket setPort(port); self)
	
	//doc SecureServer port Returns the port on which the server will listen for connections.")
	
	port := method(socket port)
	
	start := method(
		//writeln("Server start")
		self ssocks := List clone
		self newSocks := List clone
		if(usesTLS,
			socket serverOpen
			,
			socket udpOpen
			socket asyncBind(socket ipAddress)
			setDispatchCoro(self coroFor(dispatch))
			dispatchCoro setLabel("SecureServer Dispatch: " .. port asString)
			dispatchCoro resumeLater
			setCoro(Scheduler currentCoroutine)
			coro setLabel("SecureServer: " .. port asString)
		)
		if(usesTLS,
	    	while(socket isOpen,
	    	//writeln("Server serverWaitForConnection")
				//tcp, accept
		        newSocket := socket serverWaitForConnection
		        if(newSocket, 
		            //writeln("Server got socket"); 
		            s := tlsWrap(newSocket)
					s readBuffer  := Sequence clone
					s writeBuffer := Sequence clone
					s setupEvents
					ssocks append(s)
					e := try(
						if(s accept,
							handleSocket(s)
							,
							debugWriteln("socket accept failure, shutting down")
							s shutdown
							ssocks remove(s)
						)
					)
					e catch(Exception,
						debugWriteln("socket accept failure, shutting down")
						s shutdown
						ssocks remove(s)
					)
		        )
			)
			,
			while(socket isOpen,
				//udp, dispatch reads to SSLs
				newSocks = List clone
				coro pause
				if(newSocks size > 0,
					newSocks foreach(i,v,
						e := try(
						if(v udpAccept,
							handleSocket(v)
							,
							debugWriteln("socket accept failure, shutting down")
							v shutdown
							ssocks remove(v)
						)
						)
						e catch(Exception,
							debugWriteln("socket accept failure, shutting down")
							v shutdown
							ssocks remove(v)
						)
					)
					newSocks = List clone
				)
			)
	    )
	)
	dispatch := method(
		if(supportsDTLS not, 
			Exception raise("Addon built against OpenSSL older than 0.9.8; DTLS is not supported.")
			return nil
		)
    	while(socket isOpen,
			socket readEvent waitOn(socket readTimeout) ifTrue(
				debugWriteln("udp read")
				ip := udpRecvIP
				if(ip == nil,
					debugWriteln("recv from nobody")
					continue
				)
				s := secureSocketForPeer(ip)
				if(s,
					if(dispatchUdp(s) and(s coro),
						debugWriteln("resuming ssock")
						s coro resumeLater
					)
					,
					debugWriteln("creating udp ssl wrapper")
					s := dtlsWrap(ip clone)
					ssocks append(s)
					newSocks append(s)
					s udpServerSetup
					dispatchUdp(s)
					coro resumeLater
				)
				yield
			)
		)
	)

	secureSocketForPeer := method(ip,
		debugWriteln("Received from ", ip)
		s := ssocks detect(i, v, v ipAddress == ip)
		s
	)
	
	//doc SecureServer stop Stops the server if it is running. Returns self.")
	
	stop := method(
		socket close
		debugWriteln("shutting down server")
		ssocks foreach(i,v, v shutdown)
		debugWriteln("server shut down")
	)
	stopWhenUnused := method(
		while(ssocks select(isOpen) size > 0,
			yield
		)
		stop
	)
	handleSocket := method(aSocket,
		Exception raise("need to override Server handleSocket in your subclass")
	)
)

IPAddress setSlot("==", method(other, 
	(ip == other ip) and(port == other port))
)

IPAddress setSlot("realClone", IPAddress getSlot("clone"))
IPAddress setSlot("clone", method(
	c := realClone
	c setPort(port)
	c setIp(ip)
	c
))