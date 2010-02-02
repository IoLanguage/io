//metadoc VolcanoServer copyright Rich Collins 2009
//metadoc VolcanoServer license BSD revised
//metadoc VolcanoServer description A simplified version of HttpServer
//metadoc VolcanoServer category Networking

VolcanoServer := Server clone do(
	setPort(8080)
	setHost("127.0.0.1")
	
	handleSocket := method(aSocket,
		rh := VolcanoRequestHandler clone
		rh setServer(self)
		rh setSocket(aSocket)
		rh @handleRequest
		self
	)
)