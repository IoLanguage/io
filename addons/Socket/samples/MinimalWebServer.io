#!/usr/bin/env io


WebRequest := Object clone do(
	handleSocket := method(aSocket,
		aSocket streamReadNextChunk
		request := aSocket readBuffer betweenSeq("GET ", " HTTP")
		if(request endsWithSeq("/"), request appendSeq("index.html"))
		request = request exSlice(1)
		f := File with(request)
		if (f exists, aSocket streamWrite(f contents), aSocket streamWrite(request .. " not found"))
		writeln("requested ", request)
		aSocket close
	)
)

WebServer := Server clone do(
	setPort(8011)
	handleSocket := method(aSocket, WebRequest clone @handleSocket(aSocket))
)

WebServer start
