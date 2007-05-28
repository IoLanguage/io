#!/usr/bin/env io

// A simple chat server

ChatClient := Object clone
ChatClient handleSocketFromServer := method(aSocket, aServer,
  self server := aServer
  self socket := aSocket
  self name := "Unnamed"
  while(aSocket isOpen, 
    if(aSocket read, 
      text := aSocket readBuffer asString
      self handleCommand(text)
    )
    aSocket readBuffer empty
  )
  server removeClient(self)
  write("[Closed connection from ", socket host, "]\n")
)

ChatClient handleCommand := method(text,
  if (text beginsWithSeq("name:"),
      self name := text substring(6, -1) replace("\n", "") replace("\r", "")
      return
  )
  server broadcastFrom(name .. ": " .. text, self)
)

ChatClient send := method(text, socket write(text))



ChatServer := Server clone 
ChatServer clients := List clone
ChatServer init := method(self clients := List clone)

ChatServer handleSocket := method(aSocket,
  write("[Accepted connection from ", aSocket host, "]\n")
  client := ChatClient clone
  clients append(client)
  client @handleSocketFromServer(aSocket, self)
)
ChatServer removeClient := method(client, clients remove(client))

ChatServer broadcastFrom := method(text, senderClient,
  write(text)
  clients foreach(client, if (client != senderClient, client send(text)))
)


server := ChatServer 
port := 8405
write("Chat server starting on port ", port, "\n")
server setPort(port) start

