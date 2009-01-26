Sockets := Object clone

Socket do(
	//metadoc Socket category Networking

	//doc Socket bytesPerRead Returns number of bytes to read per read call.
	//doc Socket setBytesPerRead(numberOfBytes) Sets number of bytes to read per read call. Returns self.
	bytesPerRead ::= 4096
	
	//doc Socket bytesPerWrite Returns number of bytes to write per write call.
	//doc Socket setBytesPerWrite(numberOfBytes) Sets number of bytes to write per write call. Returns self.
	bytesPerWrite ::= 1024
	
	//doc Socket readTimeout Returns the length of time in seconds for read timeouts on the socket.
	//doc Socket setReadTimeout(seconds) Sets the length of time in seconds for read timeouts on the socket. Returns self.
	readTimeout ::= 60
	
	//doc Socket writeTimeout Returns the length of time in seconds for write timeouts on the socket.
	//doc Socket setWriteTimeout(seconds) Sets the length of time in seconds for write timeouts on the socket. Returns self.
	writeTimeout ::= 60

	//doc Socket connectTimeout Returns the length of time in seconds for connect timeouts on the socket.
	//doc Socket setConnectTimeout(seconds) Sets the length of time in seconds for connect timeouts on the socket. Returns self.	
	connectTimeout ::= 60
	
	//doc Socket acceptTimeout Returns the length of time in seconds for accept timeouts on the socket.
	//doc Socket setAcceptTimeout(seconds) Sets the length of time in seconds for accept timeouts on the socket. Returns self.
	acceptTimeout ::= 365*24*60*60
	
	//doc Socket ipAddress Returns the IpAddress object for the socket.
	//doc Socket setIpAddress(ipAddressObject) Sets the ipAddress for the socket. Returns self. The setHost() method should generally be used to set the host instead of this method.
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

	//doc Socket setHost(hostNameOrIpString) Set the host for the socket. Returns self on success, an Error object otherwise.
	setHost := method(ip,
		if(ip == "localhost") then(
			ip = "127.0.0.1"
		) else (
			ip at(0) isDigit ifFalse(
				//writeln("lookup = ", ip)
				ip = DNSResolver ipForHostName(ip) returnIfError
				ip ifNil(return(Error with("Could not resolve " .. ip)))
				//writeln("host ip = ", ip)
			)
		)
		ipAddress setIp(ip)
		self
	)
	
	//doc Socket host Returns the host for the socket.
	host := method(ipAddress ip)

	//doc Socket setPort(portNumber) Sets the port number for the socket, returns self.
	setPort := method(port,
	   ipAddress setPort(port)
	   self
	)

	//doc Socket port Returns the port number for the socket.
	port := method(ipAddress port)

	//doc Socket streamOpen Opens the socket in stream mode. Returns self.
	streamOpen := method(
		asyncStreamOpen returnIfError
		self
	)

	//doc Socket udpOpen Opens the socket in UDP (connectionless) mode. Returns self.
	udpOpen := method(
		asyncUdpOpen returnIfError
		self
	)

	//doc Socket connect Connects to the socket's host. Returns self on success or an Error object on error.
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

	/*doc Socket streamRead(numberOfBytes) 
	Reads numberOfBytes from the socket into the socket's readBuffer. 
	Returns self when all bytes are read or an Error object on error.
	*/
	streamRead := method(numBytes,
		total := readBuffer size + numBytes

		isOpen ifFalse(
			return(Error with("Socket closed before " .. numBytes .. " bytes could be read"))
		)
	
		while(readBuffer size < total,
			readEvent waitOn(readTimeout) returnIfError
			error := asyncStreamRead(readBuffer, total - readBuffer size) 
			/*
			writeln("error = ", error type)
			writeln("isOpen  = ", isOpen)
			writeln("isValid = ", isValid)
			*/
			isOpen ifFalse(
				Exception raise("Socket is closed")
				return(Error with("Socket closed before " .. numBytes .. " bytes could be read"))
			)
		)
		self
	)

	/*doc Socket streamWrite(buffer, optionalProgressBlock) 
	Write's buffer to the socket. 
	If optionalProgressBlock is supplied, it is periodically called with the number of bytes written as an argument.
	Returns self on success or an Error object on error.
	*/
	streamWrite := method(buffer, writeCallback,
		appendToWriteBuffer(buffer) 
		debugWriteln("socket writing [", buffer, "]")
		writeFromBuffer(writeCallback)
	)

	/*doc Socket writeFromBuffer(optionalProgressBlock) 
	Write's the contents of the socket's writeBuffer to the socket. 
	If optionalProgressBlock is supplied, it is periodically called with the number of bytes written as an argument.
	Returns self on success or an Error object on error.
	*/	
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

	/*doc Socket streamReadNextChunk(optionalProgressBlock) 
	Waits for incoming data on the socket and when found, reads any available data and returns self. 
	Returns self on success or an Error object on error or timeout.
	*/
	streamReadNextChunk := method(
		self setSocketReadLowWaterMark(1)
		//while(isOpen and e := asyncStreamRead(readBuffer, bytesPerRead), e returnIfError)
		readEvent waitOn(readTimeout) returnIfError
		while(isOpen and e := asyncStreamRead(readBuffer, bytesPerRead), e returnIfError)
		self
	)
	
	/*doc Socket streamReadWhileOpen
	Reads the stream into the socket's readBuffer until it closes.
	Returns self on success or an Error object on error.
	*/	
	streamReadWhileOpen := method(
		while(isOpen,
			streamReadNextChunk returnIfError
		)
		self
	)

	
	/*doc Socket udpReadNextChunk(ipAddress)
	Waits to receive UDP data from the specified ipAddress. 
	As soon as any data is available, it reads all of it into the socket's readBuffer.
	Returns self on success or an Error object on error.
	*/
	udpReadNextChunk := method(ipAddress,
		readEvent waitOn(readTimeout) returnIfError
		while(e := asyncUdpRead(ipAddress, readBuffer, bytesPerRead), e returnIfError)
		self
	)

	/*doc Socket udpRead(ipAddress, numBytes)
	Waits for and reads numBytes of udp data from the specified ipAddress into the socket's readBuffer.
	Returns self on success or an Error object on error.
	*/
	udpRead := method(ipAddress, numBytes,
		total := readBuffer size + numBytes

		while(readBuffer size < total,
			asyncUdpRead(ipAddress, readBuffer, bytesPerRead) returnIfError
		)
		self
	)

	//doc Socket udpWrite Same as asyncUdpWrite.
	udpWrite := getSlot("asyncUdpWrite")

	/*doc Socket serverOpen 
	Opens the socket as a stream, binds it to it's ipAddress and calls asyncListen to prepare the socket to accept connections.
	Returns self on success or an Error object on error.
	*/
	serverOpen := method(
	   streamOpen returnIfError
	   asyncBind(ipAddress) returnIfError
	   asyncListen returnIfError
		self
	)

	/*doc Socket serverWaitForConnection
	Waits for a connection or timeout. When a connection is received, this method returns the connection socket. 
	An Error object is returned on timeour or error.
	*/	
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
	
	/*doc Socket appendToWriteBuffer(aSequence)
	Appends aSequence to the write buffer if it is non-nil. Returns self.
	*/
	appendToWriteBuffer := method(buffer,
		if(buffer, writeBuffer appendSeq(buffer))
		self
	)

	
	/*doc Socket readUntilSeq(aSequence)
	Reads the socket until it's readBuffer contains aSequence, then returns a Sequence
	containing the readBuffer's contents up to (but not including) aSequence and clips that section from the readBuffer.
	*/	
	readUntilSeq := method(aSeq,
		while(readBuffer containsSeq(aSeq) not, self read)
		s := readBuffer beforeSeq(aSeq)
		readBuffer clipBeforeEndOfSeq(aSeq)
		s
	)


	/*doc Socket readBytes(numBytes)
	Reads the socket until its readBuffer is numBytes long, then returns a Sequence
	containing the first numBytes of readBuffer's contents and clips that section from the readBuffer.
	*/
	readBytes := method(numBytes,
		while(readBuffer size < numBytes, self read)
		bytes := readBuffer inclusiveSlice(0, numBytes - 1)
		readBuffer removeSlice(0, numBytes - 1)
		bytes
	)


	/*doc Socket writeMessage(aSeq)
	Writes a 4 byte uint32 in network byte order containing the size of aSeq. 
	Then writes the bytes in aSeq and returns self.
	*/
	writeMessage := method(aSeq,
		s := Sequence clone setItemType("uint32") append(aSeq size)
		s setItemType("uint8")
		s setEncoding("number")
		//writeln("writeMessage write [", s ,"]")
		streamWrite(s)
		//writeln("writeMessage write [", aSeq ,"]")
		streamWrite(aSeq)
		self
	)
		
	/*doc Socket readMessage
	Empties the readBuffer and reads a 4 byte uint32 in network byte order. 
	This number is the number of bytes in the message payload which are 
	then read into the socket's readBuffer. The readBuffer is returned.
	*/
	readMessage := method(
		readBuffer empty
		//writeln("before streamRead readBuffer size = ", readBuffer size)
		streamRead(4) raiseIfError("error in stream read")
		//writeln("after streamRead readBuffer size = ", readBuffer size)
		//writeln("[", readBuffer, "]")
		requestSize := readBuffer clone setItemType("uint32") at(0)
		readBuffer empty
		//writeln("requestSize = ", requestSize)
		readBuffer empty
		streamRead(requestSize)
		readBuffer
	)
	
	//doc Socket writeListMessage(aList) A shortcut for writeMessage(aList asEncodedList).
	writeListMessage := method(aList,
		writeMessage(aList asEncodedList)
	)
	
	//doc Socket readListMessage A shortcut for List fromEncodedList(socket readMessage).
	readListMessage := method(
		s := readMessage returnIfError
		List fromEncodedList(s)
	)
)
