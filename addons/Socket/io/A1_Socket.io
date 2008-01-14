Sockets := Object clone

Socket do(
	type := "Socket"
	docCategory("Networking")

	bytesPerRead ::= 4096
	bytesPerWrite ::= 1024
	readTimeout ::= 60
	writeTimeout ::= 60
	connectTimeout ::= 60
	acceptTimeout ::= 365*24*60*60
	ipAddress ::= IPAddress clone

	init := method(
		resend
		//writeln("Socket init ", self uniqueId)
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
		//writeln("ip = ", ip)
		ip at(0) isDigit ifFalse(DNSResolver raiseOnError(ip = DNSResolver ipForHostName(ip)))
		ipAddress setIp(ip)
		self
	)

	host := method(ipAddress host)

	setPort := method(port,
	   ipAddress setPort(port)
	   self
	)

	port := method(ipAddress port)

	checkErrno := method(
		//writeln("System errno = ", System errno)
		Socket errno ifNonNil(setError("Socket errno: " .. System errno))
	)

	setupEvents := method(
		readEvent setDescriptorId(descriptorId)
		writeEvent setDescriptorId(descriptorId)
	)

	streamOpen := method(
		asyncStreamOpen
		raiseOnError(checkErrno)
		setupEvents
		true
	)

	udpOpen := method(
		asyncUdpOpen
		checkErrno
		setupEvents
		true
	)

	setErrorIfInvalid := method(
		if(isValid == false, setError("Socket is invalid"))
		isValid not
	)

	connect := method(
		debugWriteln("Socket connect isOpen = ", isOpen)
		isOpen ifFalse(streamOpen)
		didConnect := asyncConnect(ipAddress)
		didConnect ifFalse(
			writeEvent waitOn(connectTimeout) ifFalse(
				setError("Connection timeout")
				return(false)
			)
			if(setErrorIfInvalid,
				return(false)
			)
			didConnect = asyncConnect(ipAddress)
			//checkErrno
		)
		if(didConnect not and errno == "WSA Error 10022",
			writeEvent waitOn(connectTimeout) ifFalse(
				setError("Connection timeout")
				return(false)
			)
			if(setErrorIfInvalid,
				return(false)
			)
			didConnect = asyncConnect(ipAddress)
		)
		didConnect ifTrue(return(true))
		checkErrno
		false
	)

	streamRead := method(numBytes,
		total := readBuffer size + numBytes
		//readEvent setDescriptorId(descriptorId)

		while(readBuffer size < total,
			readEvent waitOn(readTimeout) ifFalse(return false)
			while(asyncStreamRead(readBuffer, bytesPerRead), nil)
		)

		isOpen
	)

	streamWrite := method(buffer,
		n := 225280
		self setBytesPerWrite(n/2)
		self setSocketWriteBufferSize(n)
		self setSocketWriteLowWaterMark(bytesPerWrite)
		//writeln("getSocketReadLowWaterMark = ", getSocketReadLowWaterMark)
		//writeln("getSocketWriteLowWaterMark = ", getSocketWriteLowWaterMark)
		//writeEvent waitOn(writeTimeout)

		if(buffer size <= bytesPerWrite,
			//writeln("writing all ", 0, " ", buffer size)
			if(self isOpen not,
				setError("Attempted to write to closed socket")
				return(false)
			)
			writeEvent waitOn(writeTimeout) ifFalse(
				setError("Write timeout")
				return(false)
			)
			asyncStreamWrite(buffer, 0, buffer size)
			return(true)
		)

		startIndex := 0
		bytesRemaining := buffer size
		while(startIndex <= bytesRemaining,
			//writeln(startIndex)
			if(self isOpen not,
				setError("Attempted to write to closed socket")
				return(false)
			)
			writeEvent waitOn(writeTimeout) ifFalse(
				setError("Write timeout")
				return(false)
			)
			asyncStreamWrite(buffer, startIndex, bytesPerWrite)
			startIndex = startIndex + bytesPerWrite
		)
		true
	)

	streamReadNextChunk := method(
		self setSocketReadLowWaterMark(1)
		while(asyncStreamRead(readBuffer, bytesPerRead), nil)
		if(self isOpen not, return(false))
		readEvent waitOn(readTimeout) ifFalse(return(false))
		while(asyncStreamRead(readBuffer, bytesPerRead), nil)
		//writeln("streamReadNextChunk readBuffer size = ", readBuffer size)
		isOpen
	)

	streamWriteCopy := method(buffer,
		writeBuffer copy(buffer)
		streamWrite(writeBuffer)
	)

	udpReadNextChunk := method(ipAddress,
		setErrorIfInvalid ifTrue(return(false))
		readEvent waitOn(readTimeout) ifFalse(return(false))
		setErrorIfInvalid ifTrue(return(false))
		while(asyncUdpRead(ipAddress, readBuffer, bytesPerRead), setErrorIfInvalid ifTrue(return(false)))
		true
	)

	udpRead := method(ipAddress, numBytes,
		total := readBuffer size + numBytes

		while(readBuffer size < total,
			setErrorIfInvalid ifTrue(return(false))
			asyncUdpRead(ipAddress, readBuffer, bytesPerRead) //ifFalse(
				//readEvent waitOn(readTimeout)
				//asyncUdpRead(ipAddress, readBuffer, bytesPerRead)
				//nil
			//)
			setErrorIfInvalid ifTrue(return(false))
			//checkErrno
		)
		true
	)

	udpWrite := getSlot("asyncUdpWrite")

	serverOpen := method(
	   streamOpen
	   asyncBind(ipAddress)
	   asyncListen
	)

	serverWaitForConnection := method(
		newAddress := IPAddress clone
		readEvent waitOn(acceptTimeout)
		socket := asyncAccept(newAddress)
		if(socket,
			socket init
			socket setupEvents
			socket setIpAddress(newAddress)
		)
		socket
	)
)
