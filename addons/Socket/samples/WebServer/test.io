#!/usr/bin/env io

//debugCollector := 1
SocketManager setSelectTimeout(0.0)
Scheduler setSleepInterval(0)

requestCount := if(args at(0), args at(0) asNumber, 10000) 

t1 := Date clone now
lastChunkTime := Date clone now
lastPause := 0

for (i, 1, requestCount,
	socket := Socket clone setHost("127.0.0.1") setPort(8000) 
	socket setReadTimeout(.01) setConnectTimeout(.1)
	socket connect
	if (socket isOpen == nil, writeln(i, " failed connect"); continue)
	socket write("GET /\r\n")
	

	dt := (Date clone now - lastChunkTime) totalSeconds
	//writeln(i)
	if (dt > .1, 
		writeln(i, " paused for ", dt asString(1, 5), " seconds, diff: ", i - lastPause)
		lastPause = i
	)
	lastChunkTime := Date clone now

	socket close
)

dt := (Date clone now - t1) totalSeconds
writeln(requestCount, " requests in ", dt , " seconds")
writeln((requestCount / dt) floor, " requests per second")
