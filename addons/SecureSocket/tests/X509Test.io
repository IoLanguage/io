SecureServer
SecureClient
SecureSocket
Certificate

X509Test := UnitTest clone do(
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

	testCertificate := method(
		//debugOn
		debugWriteln("\n--- testCertificate")
		debugWriteln("creating server")
		sv := SecureServer clone setPort(6000) useTLS setRequiresClientCertificate(true)
		assertNotNil(sv setCAFile(ca), "server CA file not loaded successfully")
		assertNotNil(sv setCertFile(serverCert), "server cert file not loaded successfully")
		assertNotNil(sv setKeyFile(serverKey), "server key file not loaded successfully")
		sv handleSocket := method(ssock,
			ssock read(1)
			ssock shutdown
		)
		sv @start
		yield
		debugWriteln("creating client")
		cl := SecureClient clone useTLS
		assertNotNil(cl setCAFile(ca), "client CA file not loaded successfully")
		assertNotNil(cl setCertFile(clientCert), "client cert file not loaded successfully")
		assertNotNil(cl setKeyFile(clientKey), "client key file not loaded successfully")
		s := cl connectionToServer("127.0.0.1", 6000)
		debugWriteln("client created")
		assertNotEquals(s, cl)
		assertNotNil(s)
		debugWriteln("client connecting...")
		assertTrue(s connect())
		assertTrue(s isConnected)
		debugWriteln("client connected")
		cert := s certificate
		debugWriteln("cert obtained?")
		assertNotNil(cert)
		assertEquals(cert, s certificate)
		assertEquals(cert version, 1)
		assertEquals(cert serialNumber, 2)
		//Date fromString("2007-08-17 13:46:59 EDT", "%Y-%m-%d %H:%M:%S %Z")
		testDate := Date clone setYear(2007) setMonth(8) setDay(17) setHour(13) setMinute(46) setSecond(59)
		assertEquals(cert notBefore, testDate)
		testDate := Date clone setYear(2008) setMonth(8) setDay(16) setHour(13) setMinute(46) setSecond(59)
		assertEquals(cert notAfter, testDate)
		//extensions and attributes are not really tested yet since the certs I have are really trivial
		assertEquals(cert extensions slotNames size, 0)
		assertEquals(cert attributes slotNames size, 0)
		certIssuer := cert issuerName
		assertNotNil(certIssuer)
		assertEquals(certIssuer commonName, "dtls-server")
		assertEquals(certIssuer organizationName, "dtls-example")
		assertEquals(certIssuer countryName, "IN")
		assertEquals(certIssuer emailAddress, "hi2arun@gmail.com")
		assertEquals(certIssuer localityName, "CH")
		assertEquals(certIssuer stateOrProvinceName, "TN")
		certSubject := cert subjectName
		assertNotNil(certSubject)
		assertEquals(certSubject commonName, "dtlsclient")
		assertEquals(certSubject organizationName, "dtls-example")
		assertEquals(certSubject countryName, "IN")
		assertEquals(certSubject emailAddress, "hi2arun@gmail.com")
		assertEquals(certSubject stateOrProvinceName, "TN")
		debugWriteln("fetching peer cert")
		peer := s peerCertificate
		debugWriteln("peer cert obtained?")
		assertNotNil(peer)
		assertEquals(peer, s peerCertificate)
		assertEquals(peer version, 1)
		assertEquals(peer serialNumber, 1)
		testDate := Date clone setYear(2007) setMonth(8) setDay(17) setHour(13) setMinute(46) setSecond(54)
		assertEquals(peer notBefore, testDate)
		testDate := Date clone setYear(2008) setMonth(8) setDay(16) setHour(13) setMinute(46) setSecond(54)
		assertEquals(peer notAfter, testDate)
		//extensions and attributes are not really tested yet since the certs I have are really trivial
		assertEquals(peer extensions slotNames size, 0)
		assertEquals(peer attributes slotNames size, 0)
		peerIssuer := peer issuerName
		assertNotNil(peerIssuer)
		assertEquals(peerIssuer commonName, "dtls-server")
		assertEquals(peerIssuer organizationName, "dtls-example")
		assertEquals(peerIssuer countryName, "IN")
		assertEquals(peerIssuer emailAddress, "hi2arun@gmail.com")
		assertEquals(peerIssuer localityName, "CH")
		assertEquals(peerIssuer stateOrProvinceName, "TN")
		peerSubject := peer subjectName
		assertNotNil(peerSubject)
		assertEquals(peerSubject commonName, "dtlsserver")
		assertEquals(peerSubject organizationName, "dtls-example")
		assertEquals(peerSubject countryName, "IN")
		assertEquals(peerSubject emailAddress, "hi2arun@gmail.com")
		assertEquals(peerSubject stateOrProvinceName, "TN")
		s shutdown
		assertFalse(s isOpen)
		debugWriteln("finishing")
		sv stopWhenUnused
		debugWriteln("shut down")
	)
)