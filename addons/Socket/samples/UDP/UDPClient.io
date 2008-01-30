s := Socket clone setPort(9999) setHost("127.0.0.1")
s udpOpen
m := "Hello world!" asMutable
s udpWrite(s ipAddress, m, 0, m size)
