HttpResponse := Object clone do(
	socket ::= nil
	headers ::= nil
	statusCode ::= 200
	contentType ::= "text/html"
	body ::= nil
	
	init := method(
		setHeaders(Map clone)
		setBody("")
	)
	
	withSocket := method(aSocket,
		HttpResponse clone setSocket(aSocket)
	)
	
	status := method(
		statusCode asString .. " " .. StatusCodes at(statusCode asString)
	)
	
	send := method(
		socket write("HTTP/1.1 " .. status .. "\r\n")
		headers atIfAbsentPut("Content-Type", contentType)
		headers foreach(name, value, socket write(name .. ": " .. value .. "\n"))
		socket write("\n")
		socket write(body)
	)
)