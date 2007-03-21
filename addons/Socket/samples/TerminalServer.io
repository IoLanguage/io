
Terminal := Object clone
Terminal handleSocketFromServer := method(aSocket, aServer,
  while(aSocket isOpen,
    aSocket write("\nIo> ") 
    if(aSocket read,
      e := try(
        result := Lobby doString(aSocket readBuffer asString)
        if(result type == "Number", result := result asString)
        if(result type != "String", result := "<".. result type ..">")
        aSocket write(result)
      ) 
      e catch (Exception,
        aSocket write("exception: ", e name, " : ", e description)
      )
    )
    aSocket readBuffer empty
  )
)

write("starting io terminal server\n")
server := Server clone setPort(8460) 
server handleSocket := method(aSocket,
  Terminal clone @handleSocketFromServer(aSocket, self)
)
server start
