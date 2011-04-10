#!/usr/bin/env io

socketPath := "/tmp/.io_upsocket"
File with(socketPath) remove

Server := Object clone do(
    socket := Socket clone setPath(socketPath)
    start := method(
	writeln("server listing on path ", socket path)
	socket serverOpen
	socket setReadTimeout(1)
	
	s := nil
	while (s == nil, 
	    write("serverWaitForConnection\n")
	    s := socket serverWaitForConnection
	)
	writeln("server got connection ")
	s write("HELLO")
	wait(0.1)
	s read
	msg := s readBuffer asString
	if (msg == "GOODBYE", writeln("server got GOODBYE"), writeln("message failed"))
	wait(0.1)
	s close
	socket close
	Lobby @doMessage(System exit)
	wait(0.1)
    )
)

Server @start

Client := Object clone do(
    socket := Socket clone setPath(socketPath)
    start := method(
	writeln("client connecting to ", socket path)
	wait(2)
	socket connect
	if (socket isOpen, writeln("client connected"), writeln("connection failed"))
	wait(0.1)
	socket read
	msg := socket readBuffer asString
	if (msg == "HELLO", writeln("client got HELLO"), writeln("message failed"))
	socket write("GOODBYE")
	socket close
    )
)

Client @start

loop(wait(1))
