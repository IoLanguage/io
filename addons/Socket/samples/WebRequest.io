
//DNSResolver addServerAddress("128.105.2.10") 

ip := Host clone setName("www.yahoo.com") address

socket := Socket clone setHost(ip) setPort(80)
socket connect

if (socket error) then (
	writeln(socket error)
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