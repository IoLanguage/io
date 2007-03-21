
DNSResolver addServerAddress("128.105.2.10") 
ip := Host clone setName("www.yahoo.com") address
socket := Socket clone setHost(ip) setPort(80) connect 
if (socket error, write(socket error, "\n") ) 
socket write("GET /\n\n")
socket read
if (socket error, write(socket error, "\n") ) 
write("read ", socket readBuffer size, " bytes\n")
