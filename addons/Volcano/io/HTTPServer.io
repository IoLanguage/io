HTTPServer := Server clone do(
	setPort(8080)
	setHost("127.0.0.1")

	handleSocket := method(socket,
		@handleRequest(socket)
	)
	
	handleRequest := method(socket,
		parser := HTTPParser clone setParseBuffer(socket readBuffer)
		while(parser isFinished not,
			socket streamReadNextChunk ifError(e,
				writeln("Error reading next chunk: ", e description)
				socket close
				return
			)
			parser parse ifError(e,
				writeln("Error parsing request: ", e description)
				socket close
				return
			)
		)
		socket write("HTTP/1.x 200 OK
Content-Type: text/html

<html>")
	socket write("<pre>" .. parser ?body .. "</pre>")
	socket write("</html>")
		socket close
	)
)