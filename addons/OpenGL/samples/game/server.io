#!/usr/bin/env io

// --- Model -------------------------------------------------------

doFile(Path with(System launchPath, "Tank.io"))

Arena := Object clone
Arena size := 100
Arena topId := 0
Arena objects := Map clone
Arena timestep := method(
  objects foreach(i, object, 
    object timestep
    if (object x < 0, object x := size + object x)
    if (object y < 0, object y := size + object y)
    if (object x > size, object x := size - object x)
    if (object y > size, object y := size - object y)
    
    objects foreach(j, other, 
      if (object != other, object checkCollision(other))
    )
  )
)
Arena addObject := method(obj, 
  topId ++
  obj id := topId asString
  objects atPut(obj id, obj)
  GameServer addedObject(obj)
  return obj id
)

Arena removeObject := method(obj, 
  objects remove(obj id)
  GameServer removedObject(obj)
)

// --- Player -------------------------------------------------------

Player := Object clone
Player object := nil
Player setSocket := method(s, self socket := s; self)
Player setServer := method(s, self server := s; self)
Player setObject := method(s, self object := s; self)
Player run := method(
  while(socket isOpen, 
    if(socket read, 
      text := socket readBuffer asString
      self receive(text)
    )
    socket readBuffer empty
  )
  server removePlayer(self)
  write("[Closed connection from ", socket host, "]\n")
)

Player receive := method(text, 
  p := text splitNoEmpties(" ")
  action := p at(0)
  value := p at(1)
  write("Player receive('", action, "', '", value, "')\n")
  if (object) then( 
    if (action == "k", object key(value))
  ) else (
    write("requestObjectForPlayer\n")
    if (action == "k" and value == "n",
    write("requestObjectForPlayer2\n")
      self object := server requestObjectForPlayer(self)
    )
  )
)

Player send := method(text, socket write(text))

// --- GameServer -------------------------------------------------------

GameServer := Server clone 
GameServer players := List clone
GameServer init := method(self players := List clone)

GameServer handleSocket := method(aSocket,
  write("[Accepted connection from ", aSocket host, "]\n")
  player := Player clone setSocket(aSocket) setServer(self)
  players append(player)
  player @run

  Arena objects foreach(i, object,
    player send("a " .. i .. " " .. object protoName .. " \n")
  )
)

GameServer addedObject := method(object,
  self broadcast("a " .. object id .. " " .. object protoName .. " \n")
)

GameServer removedObject := method(object,
  self broadcast("r " .. object id .. "\n")
)

GameServer removePlayer := method(player, 
  write("removing player\n")
  players remove(player)
)

GameServer requestObjectForPlayer := method(player, 
  t := Tank clone
  t position x := 100
  t position y := 100
  Arena addObject(t)
  player object := t
  player send("i " .. t id)
)

GameServer broadcast := method(text, senderPlayer,
  players foreach(player, player send(text))
)

GameServer timestep := method(
  Arena objects foreach(i, object, 
    self broadcast("u ".. i .. " " .. object position x .. " " .. object position y .. " " .. object angle, "\n")
  )
)


// --- Loop -------------------------------------------------------

Timer := Object clone
Timer setDelegate := method(d, self delegate := d)
Timer period := 0.05
Timer run := method(while (1, delegate timestep; wait(period)))

port := 8888
write("Game server starting on port ", port, "\n")
GameServer setPort(port) @start
Timer setDelegate(GameServer)
Timer run

