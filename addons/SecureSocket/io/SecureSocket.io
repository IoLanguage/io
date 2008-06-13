SecureSocket do(
	type := "SecureSocket"
	docCategory("Networking")
	
    newSlot("bytesPerRead", 4096)
    newSlot("bytesPerWrite", 1024)
	newSlot("readTimeout", 60)
	newSlot("writeTimeout", 60)
    newSlot("connectTimeout", 60)
    newSlot("acceptTimeout", 60)
    newSlot("error", nil)
 
    debugWriteln := nil
    debugOff := method(self debugWriteln := nil)
    debugOn := method(self debugWriteln := getSlot("writeln"))


	//debugOn

	certificate := method(
		if(_certificate == nil,
			self _certificate := rawCertificate
		)
		_certificate
	)
	peerCertificate := method(
		if(_peerCertificate == nil,
			self _peerCertificate := rawPeerCertificate
		)
		_peerCertificate
	)
	
	asyncRead := getSlot("asyncStreamRead")
	setupEvents := method(
		self _certificate := nil
		self _peerCertificate := nil
		self readEvent   := ReadEvent clone
		self writeEvent  := WriteEvent clone
		readEvent setDescriptorId(descriptorId)
		writeEvent setDescriptorId(descriptorId)
		if(isDatagram,
			self asyncRead := self getSlot("asyncUdpRead")
		)
	)
	udpServerSetup := method(
		self _certificate := nil
		self _peerCertificate := nil
		self readBuffer  := Sequence clone
		self writeBuffer := Sequence clone
		self readEvent   := self
		self newSlot("coro", nil)
		self waitOn := method(
			if(rbioReady, debugWriteln("bio already ready"); return true)
			debugWriteln("fauxread")
			if(coro, Exception raise("Simultaneous read wait"))
			setCoro(Scheduler currentCoroutine)
			coro pause
			debugWriteln("read ready")
			setCoro(nil)
			true
		)
		self writeEvent := WriteEvent clone
		writeEvent setDescriptorId(descriptorId)
		self setSlot("asyncRead", self getSlot("asyncUdpServerRead"))
	)
		
	connectBIO := method(
        status := asyncConnectBIO
		setupEvents //we have to wait until now to set up the events.
		tries := 0
        while(status <= 0,
			if(tries > 8,
				Exception raise("unable to connect")
				return false
			)
			if(wantsWrite,
        		writeEvent waitOn(connectTimeout/8) ifFalse(tries = tries + 1)
			)
			if(wantsRead,
    			readEvent waitOn(connectTimeout/8) ifFalse(tries = tries + 1)
			)
//            raiseExceptionIfInvalid
            status = asyncConnectBIO
            //checkErrno
        )
 		//writeln("didConnect = ", didConnect)
        (status > 0) ifTrue(return true)
//        checkErrno
        Exception raise("unable to connect")
        false
	)
	
	connectSSL := method(
		if(isDatagram,
			writeEvent waitOn(connectTimeout/8)
		)
        status := asyncConnectSSL
		tries := 0
        while((isConnected not) and(wantsRead or(wantsWrite)),
			if((tries > 8) or(isOpen not), return false)
			if(wantsWrite,
				debugWriteln("connection waiting on write")
            	writeEvent waitOn(connectTimeout/8) ifFalse(tries = tries + 1)
			)
			if(wantsRead,
				debugWriteln("connection waiting on read")
        		readEvent waitOn(connectTimeout/8) ifFalse(tries = tries + 1)
			)
			debugWriteln("reconnect attempt")
//            raiseExceptionIfInvalid
            status = asyncConnectSSL
			if(validateConnection != ValidationOkay,
				shutdown
				return false
			)
            //checkErrno
        )
 		//writeln("didConnect = ", didConnect)
        (status > 0) ifTrue(
			debugWriteln("connect validation ", validateConnection)
			if(validateConnection != ValidationOkay,
				shutdown
				return false
			)
			return true
		)
//        checkErrno
        false
	)
	
	validateConnection := method(
		v := rawValidate
		if(v != ValidationOkay,
			debugWriteln("validation failure, shutting down")
			self shutdown
			return v
		)
		ValidationOkay
	)
    
	connect := method(
		self readBuffer  := Sequence clone
		self writeBuffer := Sequence clone
		debugWriteln("Socket connect isOpen = ", isConnected)
		setupEvents
		if(isConnected, return true)
		if(isDatagram not, connectBIO ifFalse(return false))
		connectSSL
	)

	read := method(numBytes,
		isOpen ifFalse(Exception raise("Read attempted on closed socket."))
		total := readBuffer size + numBytes
		tries := 0
		while(readBuffer size < total, 
			if(isOpen not,
				debugWriteln("not open. so far, read ", readBuffer)
				return false
			)
			debugWriteln("trying read to ", total)
			if(tries > 8, Exception raise("socket read timeout; aborting"); return false)
			if(wantsWrite,
				debugWriteln("read wants write")
            	writeEvent waitOn(readTimeout/8) ifFalse(tries = tries + 1)
//				continue
			)
			if(wantsRead,
				debugWriteln("read wants read")
				readEvent waitOn(readTimeout/8) ifFalse(tries = tries + 1)
//				continue
			)
			readEvent waitOn(readTimeout/8) ifFalse(tries = tries + 1)
	    	while(asyncRead(readBuffer, bytesPerRead), debugWriteln("read to ", readBuffer size ))
		)
		readBuffer size >= total
	)

    write := method(buffer,
		isOpen ifFalse(Exception raise("Write attempted on closed socket."))
        n := 225280    
        self setBytesPerWrite(n/2)
        self setSocketWriteBufferSize(n)
        self setSocketWriteLowWaterMark(bytesPerWrite)
        //writeln("getSocketReadLowWaterMark = ", getSocketReadLowWaterMark)
        //writeln("getSocketWriteLowWaterMark = ", getSocketWriteLowWaterMark)
        writeEvent waitOn(writeTimeout)

        if(buffer size <= bytesPerWrite,
            if(self isConnected not, Exception raise("attempt to write to closed socket"))
			debugWriteln("trying write")
            success := asyncWrite(buffer, 0, buffer size)
			tries := 0
			while(success not and(wantsRead or(wantsWrite)),
				debugWriteln("trying write")
				if(tries > 8, Exception raise("socket write timeout; aborting"); return 0)
				if(wantsWrite,
					debugWriteln("write waiting on write" interpolate)
        			writeEvent waitOn(writeTimeout/8) ifFalse(tries = tries + 1; continue)
					if(wantsWrite,
						debugWriteln("write occurred, but socket #{descriptorId} still wants write" interpolate)
					)
//					continue
				)
				if(wantsRead,
					debugWriteln("write waiting on read")
    				readEvent waitOn(writeTimeout/8) ifFalse(tries = tries + 1; continue)
//					continue
				)
           		success = asyncWrite(buffer, 0, buffer size)
			)
            return
        )
        
        startIndex := 0
        bytesRemaining := buffer size 
		tries := 0
        while(startIndex <= bytesRemaining,
        	if(self isConnected not, Exception raise("attempt to write to closed socket"))
			debugWriteln("trying long write")
			if(tries > 8, Exception raise("socket write timeout; aborting"); return 0)
			if(wantsWrite,
       			writeEvent waitOn(writeTimeout/8) ifFalse(tries = tries + 1)
				continue
			)
			if(wantsRead,
   				readEvent waitOn(writeTimeout/8) ifFalse(tries = tries + 1)
				continue
			)
            //writeln(startIndex)
       		asyncWrite(buffer, startIndex, bytesPerWrite)
            startIndex = startIndex + bytesPerWrite
        )
		isConnected
        //writeEvent waitOn(writeTimeout)
        //write("."); File standardOutput flush
    )

	accept := method(
		debugWriteln("accepting...")
		status := asyncAccept
		debugWriteln("accept status ",  status asString)
		tries := 0
	    while((status == -1) and(wantsRead or(wantsWrite)),
			if((tries > 8) or(isOpen not), return false)
			if(wantsWrite,
				debugWriteln("accept waiting on write")
            	writeEvent waitOn(acceptTimeout/8) ifFalse(tries = tries + 1)
			)
			if(wantsRead,
				debugWriteln("accept waiting on read")
				readEvent waitOn(acceptTimeout/8) ifFalse(tries = tries + 1)
			)
			debugWriteln("trying accept")
		    status = asyncAccept
			debugWriteln("accept status ", status asString)
			if(validateConnection != ValidationOkay,
				shutdown
				return false
			)
		)
        (status > 0) ifTrue(
			debugWriteln("accept validation ", validateConnection)
			if(validateConnection != ValidationOkay,
				debugWriteln("Validation failure, shutting down")
				shutdown
				return false
			)
			return true
		)
		debugWriteln("accept failure")
		shutdown
        false
	)
	udpAccept := method(
		debugWriteln("accepting...")
		status := asyncAccept
		debugWriteln("accept status ",  status asString)
		tries := 0
	    while((status == -1) and(wantsRead or(wantsWrite)),
			if((tries > 8) or(isOpen not), return false)
			if(wantsWrite,
				debugWriteln("accept waiting on write")
            	writeEvent waitOn(acceptTimeout/8) ifFalse(tries = tries + 1)
			)
			if(wantsRead,
				debugWriteln("accept waiting on read")
				readEvent waitOn(acceptTimeout/8) ifFalse(tries = tries + 1)
			)
			debugWriteln("trying accept")
		    status = asyncAccept
			debugWriteln("accept status ", status asString)
			if(validateConnection != ValidationOkay,
				debugWriteln("Validation failure, shutting down")
				shutdown
				return false
			)
		)
        (status > 0) ifTrue(
			debugWriteln("accept validation ", validateConnection)
			if(validateConnection != ValidationOkay,
				debugWriteln("Validation failure, shutting down")
				shutdown
				return false
			)
			return true
		)
		debugWriteln("accept failure")
        false
	)
)
