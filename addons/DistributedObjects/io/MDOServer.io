//metadoc MDOServer category Networking
/*metadoc MDOServer description
A Minimal Distributed Objects server. Example;
<pre>
DateServer := Object clone do(
	acceptedMessageNames := list("currentDate")
	currentDate := method(Date clone asNumber)
)

mdoServer := MDOServer clone 
mdoServer setHost("127.0.0.1")  setPort(8123) 
mdoServer setLocalObject(DateServer clone)
mdoServer start
</pre>
*/

MDOServer := Server clone do(
	localObject ::= nil
	handleSocket := method(aSocket,
		//writeln("MDOServer got connection")
		MDOConnection clone setLocalObject(localObject) setSocket(aSocket) @receiveLoop
	)
)
