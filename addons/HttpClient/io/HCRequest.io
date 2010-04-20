HCRequest := Object clone do(
	url ::= nil
	
	httpMethod ::= "GET"
	
	headers ::= method(
		headers = Map clone
		headers atPut("User-Agent", "Io-HttpClient http://iolanguage.com/")
		headers atPut("Accept", "*/*")
		headers atPut("Connection", "close")
	)
	
	with := method(url,
		self clone setUrl(url)
	)
	
	host := method(
		url host
	)
	
	port := method(
		url port
	)
	
	resource := method(
		url resource
	)
	
	connection := method(
		HCConnection clone setRequest(self)
	)
	
	hasHeader := method(name,
		headers hasKey(name)
	)
	
	setHeader := method(name, value,
		headers atPut(name, value)
	)
	
	header := method(name,
		headers atPut(name)
	)
)