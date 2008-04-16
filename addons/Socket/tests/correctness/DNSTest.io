Socket

DNSTest := UnitTest clone do(
	testDNSResolver := method(
		DNSResolver emptyCache
		
		ip := DNSResolver ipForHostName("www.yahoo.com")
		assertFalse(ip isError)
		assertNotNil(ip)
		
		ip := DNSResolver ipForHostName("www.ashasdjhasdhaslkhasdladasjdasjdh.com")
		assertTrue(ip isError)
	)
		
	testHost := method(
		DNSResolver emptyCache
		
		ip := Host clone setName("www.yahoo.com") address
		assertFalse(ip isError)
		assertNotNil(ip)
		
		ip := Host clone setName("www.ashasdjhasdhaslkhasdladasjdasjdh.com") address
		assertTrue(ip isError)
	)
)