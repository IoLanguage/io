#!/usr/bin/env io


//DNSResolver addServerAddress("128.105.2.10") 

ip := Host clone setName("www.yahoo.com") address

if(ip isError not,
	socket := Socket clone setHost(ip) setPort(80)
	e := socket connect

	if (e isError) then (
		writeln(e message)
	) else (
		socket write("GET /\n\n")

		response := ""
		while(socket isOpen,
			socket streamReadNextChunk
			response = response .. (socket readBuffer)
		)

		writeln(response)
		writeln("\nReceived ", response size, " bytes")
	)
,
	ip message println
)