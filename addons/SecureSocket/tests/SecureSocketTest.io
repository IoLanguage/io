SecureSocket
SecureClient
SecureServer

SecureSocketTest := UnitTest clone do(
    debugWriteln := nil
    debugOff := method(self debugWriteln := nil)
    debugOn := method(self debugWriteln := getSlot("writeln"))
    	
	serverCert := Directory currentWorkingDirectory .. "/addons/SecureSocket/tests/server/dtls.pem"
	serverKey := Directory currentWorkingDirectory .. "/addons/SecureSocket/tests/server/dtls.key"
	clientCert := Directory currentWorkingDirectory .. "/addons/SecureSocket/tests/client/dtlsc.pem"
	clientKey := Directory currentWorkingDirectory .. "/addons/SecureSocket/tests/client/dtlsc.key"
	revokeClientCRL := Directory currentWorkingDirectory .. "/addons/SecureSocket/tests/revokeClient.crl"
	revokeServerCRL := Directory currentWorkingDirectory .. "/addons/SecureSocket/tests/revokeServer.crl"
	ca := Directory currentWorkingDirectory .. "/addons/SecureSocket/tests/server/dtlsCA.pem"
	
	runServer := method(label, port, isTLS, hasCert, requiresCert, usesCRL, shouldEstablish, concurrent,
		//debugOn
		coro := Scheduler currentCoroutine 
		debugWriteln("--- Server: " .. label .. " coro ", coro uniqueId)		
		coro setLabel("S_" .. coro uniqueId)
		
		sv := SecureServer clone setPort(port)
		if(isTLS,
			sv useTLS
			,
			sv useDTLS
		)
		sv setRequiresClientCertificate(requiresCert)
		assertNotNil(sv setCAFile(ca), "server CA file not loaded successfully")
		if(hasCert,
			assertNotNil(sv setCertFile(serverCert), "server cert file not loaded successfully")
			assertNotNil(sv setKeyFile(serverKey), "server key file not loaded successfully")
		)
		if(usesCRL,
			assertNotNil(sv setCRLFile(revokeClientCRL), "server crl file not loaded successfully")
		)
		sv debugWriteln := self getSlot("debugWriteln")
		sv handler := self
		if(concurrent,
			sv handleSocket := method(ssock,
				debugWriteln("server async handling socket")
				self coroFor(handler handleSock(ssock)) resumeLater
				yield
			)
			,
			sv handleSocket := method(ssock,
				debugWriteln("server handling socket")
				handler handleSock(ssock)
			)
		)
		if(shouldEstablish,
			self handleSock := method(ssock,
				debugWriteln("server really handling socket ", ssock ipAddress asString)
				ssock read(4) ifFalse(Exception raise("Input timeout"))
				debugWriteln("server received ", ssock readBuffer, " from ", ssock ipAddress asString)
				(ssock readBuffer slice(0, 4) == "test") ifFalse(Exception raise("Incorrect input: " .. ssock readBuffer))
				yield
				debugWriteln("server writing ", ssock readBuffer, " to ", ssock ipAddress asString)
				ssock write(ssock readBuffer)
				debugWriteln("waiting for client ", ssock ipAddress asString, " shutdown")
				ssock read(1) ifTrue(Exception raise("More data than expected"))
				debugWriteln("client ", ssock ipAddress asString, " dropped")
			)
			,
			self handleSocket := method(ssock,
				debugWriteln("server really handling socket")
				yield
				Exception raise("Connection should not have been established")
			)
		)
		debugWriteln("server waiting for connections on ", port)
		sv @start
		yield
		sv
	)
	runClient := method(label, port, isTLS, hasCert, usesCRL, shouldEstablish,
		//debugOn
		debugWriteln("--- Client: " .. label .. " coro ", Scheduler currentCoroutine uniqueId)
		coro := Scheduler currentCoroutine 
		coro setLabel("C_" .. coro uniqueId)
		cl := SecureClient clone
		if(isTLS,
			cl useTLS
			,
			cl useDTLS
		)
		assertNotNil(cl setCAFile(ca), "client CA file not loaded successfully")
		if(hasCert,
			assertNotNil(cl setCertFile(clientCert), "client cert file not loaded successfully")
			assertNotNil(cl setKeyFile(clientKey), "client key file not loaded successfully")
		)
		if(usesCRL,
			assertNotNil(cl setCRLFile(revokeServerCRL), "client crl file not loaded successfully")
		)
		s := cl connectionToServer("127.0.0.1", port)
		debugWriteln("client created")
		assertNotEquals(s, cl)
		assertNotNil(s)
		debugWriteln("client connecting...")
		if(shouldEstablish,
			assertTrue(s connect())
			assertTrue(s isConnected)
			debugWriteln("client connected")
			debugWriteln("client writing test")
			yield
			s write("test")
			yield
			assertTrue(s read(4))
			debugWriteln("client received " .. s readBuffer)
			assertTrue(s readBuffer containsSeq("test"))
			,
			assertRaisesException(s connect())
			assertFalse(s isConnected)
		)
		debugWriteln("shutting down client")
		s shutdown
		debugWriteln("client shut down")
		yield
		cl
	)
	testTLS := method(
		s := runServer("TLS Normal Session", 6000, true, true, true, false, true)
		runClient("TLS Normal Session", 6000, true, true, false, true)
		s stopWhenUnused
	)
	testTLSSuccessiveClients := method(
		s := runServer("TLS Successive Clients", 6000, true, true, true, false, true)
		runClient("TLS Successive Client 1", 6000, true, true, false, true)
		runClient("TLS Successive Client 2", 6000, true, true, false, true)
		runClient("TLS Successive Client 3", 6000, true, true, false, true)
		s stopWhenUnused
	)
	testTLSConcurrentClients := method(
		s := runServer("TLS Concurrent Clients", 6000, true, true, true, false, true, true)
		yield
		@runClient("TLS Concurrent Client 1", 6000, true, true, false, true)
		yield
		@runClient("TLS Concurrent Client 2", 6000, true, true, false, true)
		yield
		runClient("TLS Concurrent Client 3", 6000, true, true, false, true)
		yield
		s stopWhenUnused
	)
	testTLSRevokedServer := method(
		s := runServer("TLS Revoked Server Certificate", 6000, true, true, true, false, false)
		runClient("TLS Revoked Server Certificate", 6000, true, true, true, false)
		s stopWhenUnused
	)
	testTLSRevokedClient := method(
		s := runServer("TLS Revoked Client Certificate", 6000, true, true, true, true, false)
		runClient("TLS Revoked Client Certificate", 6000, true, true, false, false)
		s stopWhenUnused
	)
	testTLSNoServerCert := method(
		s := runServer("TLS No Server Certificate", 6000, true, false, true, false, false)
		runClient("TLS No Server Certificate", 6000, true, true, false, false)
		s stopWhenUnused
	)
	testTLSNoClientCert := method(
		s := runServer("TLS No Client Certificate", 6000, true, true, true, false, false)
		runClient("TLS No Client Certificate", 6000, true, false, false, false)
		s stopWhenUnused
	)
	testTLSNoClientCertRequested := method(
		s := runServer("TLS No Client Certificate Requested", 6000, true, true, false, false, true)
		runClient("TLS No Client Certificate Requested", 6000, true, false, false, true)
		s stopWhenUnused
	)
	if(SecureSocket supportsDTLS,
		testDTLS := method(
			s := runServer("DTLS Normal Session", 6000, false, true, true, false, true)
			runClient("DTLS Normal Session", 6000, false, true, false, true)
			s stopWhenUnused
		)
		testDTLSConcurrentClients := method(
			s := runServer("DTLS Concurrent Clients", 6000, false, true, true, false, true, true)
			yield
			@runClient("DTLS Concurrent Client 1", 6000, false, true, false, true)
			yield
			@runClient("DTLS Concurrent Client 2", 6000, false, true, false, true)
			yield
			runClient("DTLS Concurrent Client 3", 6000, false, true, false, true)
			yield
			s stopWhenUnused
		)
		testDTLSSuccessiveClients := method(
			s := runServer("DTLS Successive Clients", 6000, false, true, true, false, true)
			runClient("DTLS Successive Client 1", 6000, false, true, false, true)
			runClient("DTLS Successive Client 2", 6000, false, true, false, true)
			runClient("DTLS Successive Client 3", 6000, false, true, false, true)
			s stopWhenUnused
		)
		testDTLSRevokedServer := method(
			s := runServer("DTLS Revoked Server Certificate", 6000, false, true, true, false, false)
			runClient("DTLS Revoked Server Certificate", 6000, false, true, true, false)
			s stopWhenUnused
		)
		testDTLSRevokedClient := method(
			s := runServer("DTLS Revoked Client Certificate", 6000, false, true, true, true, false)
			runClient("DTLS Revoked Client Certificate", 6000, false, true, false, false)
			s stopWhenUnused
		)
		testDTLSNoServerCert := method(
			s := runServer("DTLS No Server Certificate", 6000, false, false, true, false, false)
			runClient("DTLS No Server Certificate", 6000, false, true, false, false)
			s stopWhenUnused
		)
		testDTLSNoClientCert := method(
			s := runServer("DTLS No Client Certificate", 6000, false, true, true, false, false)
			runClient("DTLS No Client Certificate", 6000, false, false, false, false)
			s stopWhenUnused
		)
		testDTLSNoClientCertRequested := method(
			s := runServer("DTLS No Client Certificate Requested", 6000, false, true, false, false, true)
			runClient("DTLS No Client Certificate Requested", 6000, false, false, false, true)
			s stopWhenUnused
		)
	)
)


