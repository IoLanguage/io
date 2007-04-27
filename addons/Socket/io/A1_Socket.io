Sockets := Object clone
	
Socket do(
	type := "Socket"
	docCategory("Networking")
	
    newSlot("bytesPerRead", 4096)
    newSlot("bytesPerWrite", 1024)
    newSlot("readTimeout", 60)
    newSlot("writeTimeout", 60)
    newSlot("connectTimeout", 60)
    newSlot("acceptTimeout", 365*24*60*60)
    newSlot("error", nil)
    newSlot("ipAddress", IPAddress clone)
 
    debugWriteln := nil
    debugOff := method(self debugWriteln := nil)
    debugOn := method(self debugWriteln := getSlot("writeln"))

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
		ip at(0) isDigit ifFalse(ip = DNSResolver ipForHostName(ip))
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
		Socket errno ifNonNil(Exception raise("Socket errno: " .. System errno))
	)

	setupEvents := method(
		readEvent setDescriptorId(descriptorId)
		writeEvent setDescriptorId(descriptorId)
	)
    
    streamOpen := method(
        asyncStreamOpen
        checkErrno
        setupEvents
        true
    )
    
    udpOpen := method(
        asyncUdpOpen
        checkErrno
        setupEvents
		true
    )
    
    raiseExceptionIfInvalid := method(
    	if(self isValid == false, Exception raise("invalid socket"); writeln("invalid socket"); System exit)
    )
    
	connect := method(
		debugWriteln("Socket connect isOpen = ", isOpen)
	    isOpen ifFalse(streamOpen)
        didConnect := asyncConnect(ipAddress)
        didConnect ifFalse(
            writeEvent waitOn(connectTimeout) ifFalse(Exception raise("connection timeout"))
            raiseExceptionIfInvalid
            didConnect = asyncConnect(ipAddress)
            //checkErrno
        )
 		//writeln("didConnect = ", didConnect)
        didConnect ifTrue(return true)
        checkErrno
        Exception raise("unable to connect to " .. ipAddress ip .. " on port " .. ipAddress port)
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
            if(self isOpen not, Exception raise("attempt to write to closed socket"))
            writeEvent waitOn(writeTimeout)
            asyncStreamWrite(buffer, 0, buffer size) 
            return
        )
        
        startIndex := 0
        bytesRemaining := buffer size 
        while(startIndex <= bytesRemaining,
            //writeln(startIndex)
            if(self isOpen not, Exception raise("attempt to write to closed socket"))
            writeEvent waitOn(writeTimeout) ifFalse(break)
            asyncStreamWrite(buffer, startIndex, bytesPerWrite)
            startIndex = startIndex + bytesPerWrite
        )
        //writeEvent waitOn(writeTimeout)
        //write("."); File standardOutput flush
    )

	streamReadNextChunk := method(
        self setSocketReadLowWaterMark(1)
        readEvent waitOn(readTimeout) ifFalse(return false)
        while(asyncStreamRead(readBuffer, bytesPerRead), nil)
        //writeln("streamReadNextChunk readBuffer size = ", readBuffer size)
		isOpen
	)

    streamWriteCopy := method(buffer, 
		writeBuffer copy(buffer)
		streamWrite(writeBuffer)
	)
	
	udpReadNextChunk := method(ipAddress, 
		raiseExceptionIfInvalid
		readEvent waitOnOrExcept(readTimeout)
		if(readEvent isTimeout, write("udpReadNextChunk readTimeout"))
		raiseExceptionIfInvalid
		while(asyncUdpRead(ipAddress, readBuffer, bytesPerRead), raiseExceptionIfInvalid; nil)
		true
	)
			
	udpRead := method(ipAddress, numBytes, 
		total := readBuffer size + numBytes

		while(readBuffer size < total, 
			raiseExceptionIfInvalid
		    asyncUdpRead(ipAddress, readBuffer, bytesPerRead) ifFalse(
                //readEvent waitOn(readTimeout)
                //asyncUdpRead(ipAddress, readBuffer, bytesPerRead)
                nil
            )
		    raiseExceptionIfInvalid
		    //checkErrno
		)
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
