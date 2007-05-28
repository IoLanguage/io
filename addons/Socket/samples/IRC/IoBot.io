#!/usr/bin/env io

Feed := Object clone do(
    init := method(
        self lastItemGuid := nil
    )
    newSlot("url")
    lastItemGuid := nil
    hasNewItem := method(
        writeln("fetching ", url)
        x := URL with(url) fetch asXML asObject 
        x := if(x ?xml, x xml, x) 
        self item := x rss channel items first
        writeln("done")
        if(item guid != lastItemGuid, lastItemGuid = item guid; true, false)
    )
    message := method(messagePrefix .. " '" .. item title .. "' by " .. item author .. " " .. item link)
)

IoMailingListFeed := Feed clone do(
    setUrl("http://rss.groups.yahoo.com/group/iolanguage/rss")
    messagePrefix := "new io mailing list post:"
)

IoGoogleFeed := Feed clone do(
    setUrl("http://blogsearch.google.com/blogsearch_feeds?hl=en&q=link:iolanguage.com&ie=utf-8&num=10&output=rss")
    messagePrefix := "google blog search found new page:"
)

IRCClient := Object clone do(
    socket := Socket clone
    newSlot("nickName", "ioalertbot")
    newSlot("userName", nickName)
    newSlot("userEmail", nickName .. "@foo.com")
    newSlot("channel", "io")
    feeds := list(IoMailingListFeed)

    login := method(
        socket setReadTimeout(60*60*24)
        socket setHost("irc.freenode.net") setPort(6667) connect
        if (socket error, writeln(socket error); exit)
        socket streamWrite("NICK " .. nickName .. "\r\n")
        socket streamWrite("USER " .. userName .. " 0 0 " .. userEmail .. "\r\n")
        socket streamWrite("JOIN #" .. channel .. "\r\n")    
        socket read
        writeln("login response: \n", socket readBuffer)
        socket readBuffer empty
    )
    
    run := method(
        login
        writeln("connected\n")
        
        while (socket isOpen,

            writeln(socket readBuffer)
            feeds foreach(feed, if(feed hasNewItem, say(feed message)))
            socket readBuffer empty
            
            wait(60*5)
            writeln("----------")
        )
    )
    
    say := method(msg,
          if(msg, socket streamWrite("PRIVMSG #" .. channel .. " : " .. msg .. "\r\n"))
    )
    
    handleMessage := method(msg,
        msg = msg afterSeq("PRIVMSG #") afterSeq(":")
        
        if (msg containsSeq(nickName .. ":"), 
            msg replaceSeq(nickName .. ":", "")	
            writeln("msg1 = ", msg)
            msg lstrip
            return msg
        )
        
        return nil
    )

)

IRCClient run


