Server := Object clone do(
//metadoc Server category Networking
/*metadoc Server description 
The Server object provides a simple interface for running a server. 
You just need to set the port and define a handleSocket method. Here's an example of an echo server:

<pre>
Echo := Object clone
Echo handleSocketFromServer := method(aSocket, aServer,
  write("[Got echo connection from ", aSocket host, "]\n")
  while(aSocket isOpen,
   if(aSocket read, aSocket write(aSocket readBuffer asString))
   aSocket readBuffer empty
  )
  write("[Closed ", aSocket host, "]\n")
)

write("[Starting echo server on port 8456]\n")
server := Server clone setPort(8456)
server handleSocket := method(aSocket,
  Echo clone @handleSocketFromServer(aSocket, self)
)
server start
</pre>

<b>Notes</b><p>

Io's use of lightweight threading and select for dealing with sockets makes for 
servers that are much more efficient (both memory and cpu wise) than those written 
with kernel threads and socket polling.
*/

	init := method(
		self socket := Socket clone setPort(80) setReadTimeout(356*24*60*60)
		self isRunning := false
		self
	)

	//doc Server setHost(hostName) Sets the hostName. Returns self.
	setHost := method(host, socket setHost(host); self)

	//doc Server setPort(aNumber) Sets the port on which the server will listen  for connections. Returns self.
	setPort := method(port, socket setPort(port); self)

	//doc Server port Returns the port on which the server will listen for connections.
	port := method(socket port)

	//doc Server setPath(path) Sets the path (for Unix Domain sockets) on which the server will listen  for connections. Returns self.
	setPath := method(port, socket setPath(path); self)

	/*doc Server start
	Starts the server. This method will not return until server is stopped, 
	so you may want to send the start message as an asynchronous message. 
	Returns self or an Error, if one occurs.
	*/

	start := method(
		//writeln("Server started")
		isRunning = true
		socket serverOpen returnIfError
		while(isRunning,
			handleSocket(socket serverWaitForConnection ifError(continue))
		)
		//writeln("Server stopped")
		self
	)

	//doc Server stop Stops the server if it is running. Returns self.
	stop := method(
		isRunning = false
		socket close
	)
	
	/*doc Server handleSocket(aSocket) 
	This method is called when the server accepts a new socket. The new socket is passed as the argument.
	Override this method in your own server subclass. The default implementation raises an exception.
	*/
	handleSocket := method(aSocket,
		Exception raise("You must override Server handleSocket in your subclass")
	)
	
)

