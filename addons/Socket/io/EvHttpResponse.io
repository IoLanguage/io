EvHttpResponse := Object clone do(
	headers := Map clone atPut("Content-Type", "text/html")
	statusCode := 200
	data := "" asMutable
	
	init := method(
		headers = headers clone
		data = data clone
	)
	
	setHeader := method(name, value,
		headers atPut(name, value)
		self
	)
	
	addHeader := getSlot("setHeader")
	
	newCookie := method(
		EvHttpCookie clone setResponse(self)
	)
)