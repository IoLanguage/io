Socket

DNSTest := UnitTest clone do(

	testDNSResolver := method(
		ip := DNSResolver ipForHostName("www.yahoo.com")
		assertTrue(ip != false)
	)
		
	testHost := method(
		ip := Host clone setName("www.yahoo.com") address
		assertTrue(ip != false)
	)
)