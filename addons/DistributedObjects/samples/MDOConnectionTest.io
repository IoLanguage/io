/*
con := MDOConnection clone setHost("127.0.0.1") setPort(8000) connect
con @receiveLoop
r := con remoteObject increment(1)
writeln("r = ", r)
*/