

s := Socket clone setPort(9999) setHost("127.0.0.1")
s setReadTimeout(1000)
s udpOpen
s asyncBind(s ipAddress)
while(s isOpen not, nil)
m := s udpReadNextChunk(s ipAddress)
writeln("server got message: ", m)

