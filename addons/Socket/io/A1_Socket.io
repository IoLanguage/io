Sockets := Object clone

Socket do(
	//metadoc Socket category Networking

	bytesPerRead ::= 4096
	bytesPerWrite ::= 1024
	readTimeout ::= 60
	writeTimeout ::= 60
	connectTimeout ::= 60
	acceptTimeout ::= 365*24*60*60
	ipAddress ::= IPAddress clone

	init := method(
		resend
		self ipAddress   := ipAddress clone

		self readBuffer  := Sequence clone
		self writeBuffer := Sequence clone

		self readEvent   := ReadEvent clone
		self writeEvent  := WriteEvent clone

		self read  := self getSlot("streamReadNextChunk")
		self write := self getSlot("streamWrite")
		self
	)

	setHost := method(ip,
		ip at(0) isDigit ifFalse(
			ip = DNSResolver ipForHostName(ip) returnIfError
			ip ifNil(return(Error with("Could not resolve " .. ip)))
		)
		ipAddress setIp(ip)
		self
	)

	host := method(ipAddress host)

	setPort := method(port,
	   ipAddress setPort(port)
	   self
	)

	port := method(ipAddress port)

	streamOpen := method(
		asyncStreamOpen returnIfError
		self
	)

	udpOpen := method(
		asyncUdpOpen returnIfError
		self
	)

	connect := method(
		debugWriteln("Socket connect isOpen = ", isOpen)
		isOpen ifFalse(streamOpen returnIfError)
		asyncConnect(ipAddress) returnIfError ifNil(
			writeEvent waitOn(connectTimeout) returnIfError
			asyncConnect(ipAddress) returnIfError ifNil(
				if(System platform containsAnyCaseSeq("windows"),
					//Some versions of WinSock return an event for select() prematurely
					writeEvent waitOn(connectTimeout) returnIfError
					asyncConnect(ipAddress) returnIfError ifNil(
						return(Error with("Failed to connect"))
					)
				,
					return(Error with("Failed to connect"))
				)
			)
		)
		self
	)

	streamRead := method(numBytes,
		total := readBuffer size + numBytes

		while(readBuffer size < total,
			readEvent waitOn(readTimeout) returnIfError
			streamReadNextChunk returnIfError
			isClosed ifTrue(
				return(Error with("Socket closed before " .. numBytes .. " bytes could be read"))
			)
		)
		self
	)

	streamWrite := method(buffer, writeCallback,
		appendToWriteBuffer(buffer) writeFromBuffer(writeCallback)
	)
	
	writeFromBuffer := method(writeCallback,
		n := 225280
		self setBytesPerWrite(n/2)
		self setSocketWriteBufferSize(n)
		self setSocketWriteLowWaterMark(bytesPerWrite)

		while(writeBuffer isEmpty not,
			writeEvent waitOn(writeTimeout) returnIfError
			if(writeCallback,
				sizeBefore := writeBuffer size
				asyncStreamWrite(writeBuffer, 0, bytesPerWrite) returnIfError
				writeCallback call(sizeBefore - writeBuffer size)
			,
				asyncStreamWrite(writeBuffer, 0, bytesPerWrite) returnIfError
			)
		)
		self
	)

	streamReadNextChunk := method(
		self setSocketReadLowWaterMark(1)
		while(isOpen and e := asyncStreamRead(readBuffer, bytesPerRead), e returnIfError)
		readEvent waitOn(readTimeout) returnIfError
		while(isOpen and e := asyncStreamRead(readBuffer, bytesPerRead), e returnIfError)
		self
	)
	
	streamReadWhileOpen := method(
		while(isOpen,
			streamReadNextChunk returnIfError
		)
		self
	)

	udpReadNextChunk := method(ipAddress,
		readEvent waitOn(readTimeout) returnIfError
		while(e := asyncUdpRead(ipAddress, readBuffer, bytesPerRead), e returnIfError)
		self
	)

	udpRead := method(ipAddress, numBytes,
		total := readBuffer size + numBytes

		while(readBuffer size < total,
			asyncUdpRead(ipAddress, readBuffer, bytesPerRead) returnIfError
		)
		self
	)

	udpWrite := getSlot("asyncUdpWrite")

	serverOpen := method(
	   streamOpen returnIfError
	   asyncBind(ipAddress) returnIfError
	   asyncListen returnIfError
	)

	serverWaitForConnection := method(
		newAddress := IPAddress clone
		readEvent waitOn(acceptTimeout) returnIfError
		socket := asyncAccept(newAddress) returnIfError
		if(socket,
			socket setIpAddress(newAddress)
		,
			Error with("Timeout")
		)
	)
	
	appendToWriteBuffer := method(buffer,
		writeBuffer appendSeq(buffer)
		self
	)
)
