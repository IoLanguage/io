#!/usr/bin/env io

//doFile(Path with(System launchPath, "../_ioCode/DOConnection.io"))
//doFile(Path with(System launchPath, "../_ioCode/DOServer.io"))

//ip := Host setName("localhost") address
//ip := "127.0.0.1" 

Test := Object clone
Test test := method(v, return List clone append(1))
doServer := DOServer clone
doServer setRootObject(Test clone)
//doServer socket setHost(ip)
doServer setPort(8456)
doServer @start

e := try(
	con := DOConnection clone setHost("127.0.0.1") setPort(8456) connect
)
e catch(Exception,
	ip := Host setName("localhost") address
	con := DOConnection clone setHost(ip) setPort(8456) connect
)

result := con serverObject test(1)
//write("result = ", result, "\n")
r1 := result at(0)
//write("result at(0) = ", r1, "\n")
r2 := result at(1)
//write("result at(1) = ", r2, "\n")

writeln("DOTest: ", if (r1 == 1 and r2 == nil, "OK", "FAILED"))

doServer stop
con close

System exit
