VertexDB Server := Object clone do(
	//internal
	request := method(
		Request clone setRaisesOnError(raisesOnError) setHost(host) setPort(port)
	)
	
	//api
	host ::= method(Settings host)
	port ::= method(Settings port)
	
	raisesOnError ::= method(Settings raisesOnError)
	
	shutdownRequest := method(
		request setAction("shutdown")
	)
	
	shutdown := method(
		shutdownRequest results
	)
	
	backupRequest := method(
		request setAction("backup")
	)
	
	backup := method(
		backupRequest results
	)
	
	collectGarbageRequest := method(
		request setAction("collectGarbage")
	)
	
	collectGarbage := method(
		collectGarbageRequest results
	)
	
	sync := method(
		request setAction("sync") results
	)
)