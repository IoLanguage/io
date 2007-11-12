#!/usr/bin/env io

WebRequest := Object clone do(
    cache := Map clone
    handleSocket := method(socket, server,
        socket streamReadNextChunk
    	if(socket isOpen == false, return)
        request := socket readBuffer betweenSeq("GET ", " HTTP")
        //if(request beginsWithSeq("/"), request = request afterSeq("/"))
        
        data := cache atIfAbsentPut(request,
            writeln("caching ", request)
            f := File clone with(request)
            if(f exists, f contents, nil)
        )
    
        if(data,
			socket streamWrite("HTTP/1.0 200 OK\n\n")
            socket streamWrite(data)
        ,
            socket streamWrite("Not Found") 
        )
       
        socket close
        server requests append(self)
    )
)

WebServer := Server clone do(
    setPort(7777)
    socket setHost("127.0.0.1")
    requests := List clone    
    handleSocket := method(socket, 
        //w := requests pop ifNilEval(WebRequest clone) 
        WebRequest handleSocket(socket, self)
    )
) start
