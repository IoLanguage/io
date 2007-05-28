#!/usr/bin/env io


Socket write := Socket getSlot("streamWrite")

WebRequest := Object clone do(
    cache := Map clone
    handleSocket := method(socket, server,
        socket streamReadNextChunk
        request := socket readBuffer betweenSeq("GET ", " HTTP")
        if(request beginsWithSeq("/"), request = request afterSeq("/"))
        
        data := cache atPutIfAbsent(request,
            //writeln("caching ", request)
            f := File with(request)
            if(f exists, f contents, nil)
        )
    
        if(data) then(
            //writeln("sending ", data size, " bytes")
            socket streamWrite("HTTP/1.0 200 OK\n\nServer: Io\n")
            //socket streamWrite("Connection: Close\n")
            //socket streamWrite("Content-Length: " .. data size .."\n")
            //socket streamWrite("\n")
            socket streamWrite(data)
        ) else(
            //writeln("file ", request, " not found")
            socket streamWrite("Not Found") 
        )
       
       
        //writeln(socket sync)
        socket close
        server requests remove(self)
        //writeln("request done")
    )
)

WebServer := Server clone do(
    setPort(7777)
    socket setHost("10.0.0.3")
    requests := List clone    
    handleSocket := method(socket, 
        w := requests pop  
        w ifNil(w := requests append(WebRequest clone)) 
        WebRequest @handleSocket(socket, self)
    )
) start
