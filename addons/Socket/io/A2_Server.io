Server := Object clone do(
	type := "Server"
	docCategory("Networking")

	docDescription("""The Server object provides a simple interface for running a server. You just need to set the port and define a handleSocket method. Here's an example of an echo server:

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

Io's use of lightweight threading and select for dealing with sockets makes for servers that are much more efficient(both memory and cpu wise) than those written with kernel threads and socket polling. """)

	init := method(
		self socket := Socket clone setPort(80) setReadTimeout(356*24*60*60)
		self
	)

	docSlot("setHost(hostName)", "Sets the hostName. Returns self.")

	setHost := method(host, socket setHost(host); self)

	docSlot("setPort(aNumber)", "Sets the port on which the server will listen  for connections. Returns self.")

	setPort := method(port, socket setPort(port); self)

	docSlot("port", "Returns the port on which the server will listen for connections.")

	port := method(socket port)

	docSlot("start", "Starts the server. This method will not return until server is stopped, so you may want to send the start message as an asynchronous message. Returns self or an Error, if one occurs.")

	start := method(
		socket serverOpen returnIfError
		while(socket isOpen,
			handleSocket(socket serverWaitForConnection ifError(continue))
		)
	)

	docSlot("stop", "Stops the server if it is running. Returns self.")

	stop := method(socket close)
	
	handleSocket := method(aSocket,
		Exception raise("You must override Server handleSocket in your subclass")
	)
)

