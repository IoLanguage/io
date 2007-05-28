#!/usr/bin/env io


IRCClient := Object clone do(
    socket := Socket clone
    nickName := "echobot"
    userName := nickName
    userEmail := nickName .. "@foo.com"
    channel := "io2"

    login := method(
        socket setReadTimeout(60*60*24)
        socket setHost("irc.freenode.net") setPort(6667) connect
        if (socket error, writeln(socket error); exit)
        socket streamWrite("NICK " .. nickName .. "\r\n")
        socket streamWrite("USER " .. userName .. " 0 0 " .. userEmail .. "\r\n")
        socket streamWrite("JOIN #" .. channel .. "\r\n")    
        socket streamReadNextChunk
        socket readBuffer empty
    )
    
    run := method(
        login
        
        while (socket isOpen,
            socket streamReadNextChunk
            
            if (socket readBuffer size != 0, 
                reply := self handleMessage(socket readBuffer)
                
                if(reply,
                    fullReply := "PRIVMSG #" .. channel .. " : " .. reply .. "\r\n"
                    socket streamWrite(fullReply)
                )
                
                socket readBuffer empty
            )
        )
    )
    
    handleMessage := method(msg,
        msg = msg afterSeq("PRIVMSG #") afterSeq(":")
        
        if (msg containsSeq(nickName .. ":"), 
            return msg replaceSeq(nickName .. ":", "")	lstrip
        )
        return nil
    )
)

IRCClient run


