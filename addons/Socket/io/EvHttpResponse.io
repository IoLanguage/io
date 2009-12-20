EvHttpResponse := Object clone do(
	headers := Map clone
	statusCode := 200
	data := "" asMutable
	
	setHeader := method(name, value,
		headers atPut(name, value)
		self
	)
	
	addHeader := getSlot("setHeader")
	
	newCookie := method(
		EvHttpCookie clone setResponse(self)
	)
)