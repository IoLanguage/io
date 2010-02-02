EvOutResponse do(
	headers := Map clone atPut("Content-Type", "text/html")
	statusCode := 200
	data := "" asMutable
	responseMessage := "OK" // "Internal Server Error"
	
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
	
	setStatus := method(aCode,
		statusCode = aCode
		responseMessage = EvStatusCodes at(aCode asString)
		self
	)
	
	redirectTo := method(url,
		addHeader("Location", url)
		setStatus(302)
		self
	)
)